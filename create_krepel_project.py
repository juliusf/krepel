#!py -3 || python3
"""
Creates a new project using krepel in a given directory.
"""

from pathlib import *
import argparse
import subprocess
import os
import stat
import string


class ProjectTemplate(string.Template):
  delimiter = "~"


def get_args():
  """Get arguments to this script."""
  parser = argparse.ArgumentParser(description="Create a new project using krepel.")
  parser.add_argument("dest_dir",
                      type=Path,
                      help=("The directory to create the project in. "
                            "Will prompt for confirmation if the directory already exists."))
  parser.add_argument("--name",
                      help="A friendly name for the project.")
  parser.add_argument("--force",
                      action="store_true",
                      help="If set, always assume 'yes' for prompts.")
  parser.add_argument("-k", "--krepel-dir",
                      type=Path,
                      default=os.environ.get("KREPEL_DIR"),
                      help="The path to the krepel directory. This is where the project template is taken from.")
  return parser.parse_args()


def prompt_user_bool(question, *, max_attempts=3):
  """Prompt the user with a yes/no question."""
  for _ in range(max_attempts):
    prompt = input(question + " [y/n] ").strip().lower()
    if prompt:
      if prompt[0] == "n":
        return False
      if prompt[0] == "y":
        return True
  print("Could not determine what you want after {} attempts. "
        "Assuming 'no' is your answer.".format(max_attempts))
  return False


def rm_tree(top):
  """Equivalent to `rm -rf <top>`."""
  for root, dirs, files in os.walk(top.as_posix(), topdown=False):
    for name in files:
      filename = os.path.join(root, name)
      os.chmod(filename, stat.S_IWUSR)
      os.remove(filename)
    for name in dirs:
      os.rmdir(os.path.join(root, name))
  os.rmdir(top.as_posix())


def transform_template_file(*, file_path, dest_path, substitution_mapping):
  """Read a file, format the contents, and write to the destination."""
  if not dest_path.parent.exists():
    dest_path.parent.mkdir(parents=True)
  assert dest_path.parent.is_dir(), "Given destination path is not a directory!"

  content = ""
  with open(file_path.as_posix(), "r", encoding="utf-8") as the_file:
    content = the_file.read()
  content = ProjectTemplate(content).substitute(substitution_mapping)
  with open(dest_path.as_posix(), "w", encoding="utf-8") as the_file:
    the_file.write(content)


def all_files_in_tree(root):
  """Get all files within a path. They will be relative to that path."""
  all_files = []
  for parent, dirs, files in os.walk(root.as_posix(), topdown=False):
    # Make the given parent relative to `root`.
    parent = Path(parent).relative_to(root)

    # Add all files in this directory, relative to the parent.
    all_files.extend(parent / the_file for the_file in files)

    # Add all files in the directories below this one.
    for the_dir in dirs:
      all_files.extend(all_files_in_tree(Path(the_dir)))

  return all_files


def main():
  """Entry point."""
  args = get_args()
  dest = args.dest_dir

  # Replace all whitespaces with _ and make it upper case:
  # Hello World => Hello_World
  friendly_name = "_".join((args.name or dest.name).split())

  # Upper case the friendly_name for use as CMake variable names.
  # Hello_World => HELLO_WORLD
  cmake_name = friendly_name.upper()

  krepel_dir = args.krepel_dir.resolve()
  project_template_dir = (krepel_dir / "build" / "CMake" / "project_template").resolve()

  print("Creating project {}".format(friendly_name))
  if dest.is_file():
    print("Destination dir already exists and is a file! Aborting.")
    return
  if dest.exists():
    if not args.force and not prompt_user_bool("Destination already exists. Overwrite?"):
      return
    rm_tree(dest)
  dest.mkdir(parents=True)
  dest = dest.resolve()
  os.chdir(dest.as_posix())

  substitution_mapping = {
    "friendly_name" : friendly_name,
    "cmake_name" : cmake_name,
    "krepel_dir" : krepel_dir,
    "project_template_dir" : project_template_dir,
    "dest_dir" : dest,
  }

  all_files = all_files_in_tree(project_template_dir)

  for the_file in all_files:
    file_path = (project_template_dir / the_file).resolve()
    dest_path = dest / ProjectTemplate(the_file.as_posix()).substitute(substitution_mapping)
    print("", file_path, "=>", dest_path)
    transform_template_file(file_path=file_path,
                            dest_path=dest_path,
                            substitution_mapping=substitution_mapping)

  print("Your project has been created.")
  print("Please use CMake to generate a build system for your new project.")
  print("Full path: {}".format(dest.as_posix()))


if __name__ == "__main__":
  main()
