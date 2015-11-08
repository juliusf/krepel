#include <krEngine/game/defaultMainModule.h>
#include <krEngine/game/gameLoop.h>
#include <krEngine/common/utils.h>

#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/VisualStudioWriter.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>


void kr::DefaultWindow::OnClickCloseMessage()
{
  m_userRequestsClose = true;
}

kr::DefaultMainModule::DefaultMainModule()
{
  ezFileSystem::RegisterDataDirectoryFactory(ezDataDirectory::FolderType::Factory);
}

kr::DefaultMainModule::~DefaultMainModule()
{
}

void kr::DefaultMainModule::OnCoreStartup()
{
  if(ezFileSystem::AddDataDirectory(kr::cwd().GetData(), ezFileSystem::AllowWrites, ".", ".").Failed())
  {
    ezLog::Error("Failed to mount current working directory as data directory.");
  }

  if (m_windowDesc.m_Title.IsEmpty())
  {
    m_windowDesc.m_Title = m_plugin.GetPluginName();
  }

  m_htmlLog.BeginLog(ezStringBuilder{ "<.>", m_plugin.GetPluginName(), "Log.html" }, m_windowDesc.m_Title);
  ezGlobalLog::AddLogWriter({ &ezLogWriter::HTML::LogMessageHandler, &m_htmlLog });
}

void kr::DefaultMainModule::OnEngineStartup()
{
  EZ_VERIFY(m_window.Initialize(m_windowDesc).Succeeded(), "Failed to open window");

  // We own this game loop, so the following call should never fail.
  EZ_VERIFY(m_moduleLoop.addCallback("main", { &DefaultMainModule::tick, this }).Succeeded(), "Failed to register module tick function.");

  // The following call may fail if the user supplied their own "module" game loop.
  if(GlobalGameLoopRegistry::add("module", &m_moduleLoop, ezGlobalLog::GetInstance()).Failed())
  {
    ezLog::Error("Failed to register the main module's game loop. Things will probably not work as intended.");
  }
}

void kr::DefaultMainModule::OnEngineShutdown()
{
  GlobalGameLoopRegistry::remove("module", ezGlobalLog::GetInstance());

  if(m_window.Destroy().Failed())
  {
    ezLog::SeriousWarning("Failed to destroy window.");
  }
}

void kr::DefaultMainModule::OnCoreShutdown()
{
  ezGlobalLog::RemoveLogWriter({ &ezLogWriter::HTML::LogMessageHandler, &m_htmlLog });
  m_htmlLog.EndLog();
  ezFileSystem::RemoveDataDirectoryGroup(".");
}

void kr::DefaultMainModule::tick()
{
  m_window.ProcessWindowMessages();

  if(m_window.userRequestsClose())
  {
    GlobalGameLoopRegistry::setKeepTicking(false);
    return;
  }
}