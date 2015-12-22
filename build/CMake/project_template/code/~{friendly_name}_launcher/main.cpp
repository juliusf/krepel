#include <krEngine/pch.h>
#include <krEngine/game/mainModule.h>
#include <krEngine/game/gameLoop.h>

#include <Foundation/Utilities/CommandLineUtils.h>
#include <Foundation/IO/ExtendedJSONReader.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/VisualStudioWriter.h>

#define DEFAULT_MODULE_NAME "~{friendly_name}"

namespace
{
  struct JsonFileReader
  {
    ezOSFile file;
    ezJSONReader reader;
  };
}

// pData may NOT be a nullptr.
static ezStringBuilder extractValue(const ezVariantDictionary* pData, const char* key)
{
  EZ_ASSERT_DEBUG(pData != nullptr, "Unexpected nullptr.");

  ezVariant value;
  if(!pData->TryGetValue(key, value))
  {
    ezLog::Error("Unable to find key '%s'.", key);
    return ezStringBuilder();
  }

  if(value.GetType() != ezVariantType::String)
  {
    ezLog::Error("Expected a string value for the key '%s'.", key);
    return ezStringBuilder();
  }

  return ezStringBuilder(value.Get<ezString>());
}

// pData may be a nullptr.
static ezStringBuilder extractValue(const ezCommandLineUtils& cmd, const ezVariantDictionary* pData, const char* key)
{
  ezStringBuilder gamePath = cmd.GetStringOption(key);

  if(gamePath.IsEmpty() && pData != nullptr)
  {
    // Game path was not specified via command line switch.
    gamePath = extractValue(pData, key);
  }

  return gamePath;
}

static ezResult loadGame(const ezStringBuilder& name)
{
  EZ_LOG_BLOCK("Loading Game", name);

  return ezPlugin::LoadPlugin(name);
}

static ezResult unloadGame(const ezStringBuilder& name)
{
  EZ_LOG_BLOCK("Unloading Game", name);

  return ezPlugin::UnloadPlugin(name);
}

static int runGame(const ezStringBuilder& name)
{
  EZ_LOG_BLOCK("Running Game", name);

  ezStartup::StartupEngine();

  while(kr::GlobalGameLoop::keepTicking())
  {
    kr::GlobalGameLoop::tick();
  }

  ezStartup::ShutdownEngine();

  return 0;
}

int main(int argc, char* argv[])
{
  ezGlobalLog::AddLogWriter(ezLogWriter::Console::LogMessageHandler);
  KR_ON_SCOPE_EXIT{ ezGlobalLog::RemoveLogWriter(ezLogWriter::Console::LogMessageHandler); };
  ezGlobalLog::AddLogWriter(ezLogWriter::VisualStudio::LogMessageHandler);
  KR_ON_SCOPE_EXIT{ ezGlobalLog::RemoveLogWriter(ezLogWriter::VisualStudio::LogMessageHandler); };

  ezStartup::StartupCore();
  KR_ON_SCOPE_EXIT{ ezStartup::ShutdownCore(); };

  ezCommandLineUtils cmd;
  cmd.SetCommandLine(argc, const_cast<const char**>(argv));

  ezStringBuilder configFileName{ cmd.GetStringOption("--config") };

  if(configFileName.IsEmpty())
  {
    configFileName = "config.json";
  }

  const ezVariantDictionary* pData{ nullptr };
  ezExtendedJSONReader jsonReader;
  ezOSFile jsonFile;

  if (jsonFile.Open(configFileName, ezFileMode::Read).Failed())
  {
    ezLog::Info("Unable to open JSON config file.");
  }
  else
  {
    auto fileData{ EZ_DEFAULT_NEW_ARRAY(ezUInt8, static_cast<ezUInt32>(jsonFile.GetFileSize())) };
    jsonFile.Read(fileData.GetPtr(), fileData.GetCount());

    {
      ezMemoryStreamStorage mem{ fileData.GetCount() };
      ezMemoryStreamWriter{ &mem }.WriteBytes(fileData.GetPtr(), fileData.GetCount());
      if(jsonReader.Parse(ezMemoryStreamReader{ &mem }).Failed())
      {
        ezLog::SeriousWarning("Discarding config file due to error when parsing. Check for a valid JSON format.");
      }
      else
      {
        pData = &jsonReader.GetTopLevelObject();
      }
    }
  }

  auto gameName{ extractValue(cmd, pData, "--game") };
  if(gameName.IsEmpty())
  {
    gameName = DEFAULT_MODULE_NAME;
  }

  auto gameModuleName{ gameName };
  gameModuleName.Append(kr::configPostfix());

  auto result{ 0 };

  if(loadGame(gameModuleName).Failed())
  {
    ezLog::Error("Failed to load game.");
    result = 1;
  }

  if (result == 0)
  {
    // Now that the game module is loaded, re-init to Core again so the game module also gets the event.
    ezStartup::ReinitToCurrentState();

    auto result{ runGame(gameModuleName) };
    if(result != 0)
    {
      return result;
    }
  }

  if(unloadGame(gameModuleName).Failed())
  {
    ezLog::Error("Failed to unload game.");
    result = 1;
  }

  return result;
}
