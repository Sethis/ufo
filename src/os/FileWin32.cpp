#include "FileWin32.h"
#include "FileIO.h"
#include "logger.h"
#ifdef _WIN32
cFileWin32::cFileWin32(int index)
{
  Clear();
  m_Index = index;
};

cFileWin32::~cFileWin32()
{
  if(IsOpen())
  {
    LogWarning("File not closed: %s", m_Path.c_str());
    Close();
  }
};

bool cFileWin32::Open(const std::string &file, DWORD flags)
{
  Close();

  if(!FFileOpenFlags::Valid(flags))
  {
    LogError("Invalid flags passed");
    return false;
  }

  if(file.empty())
  {
    LogError("Invalid param");
    return false;
  }

  m_Path = file;
  m_Flags = flags;

  m_Handle = CreateFileA(m_Path.c_str(), FFileOpenFlags::ToDesiredAccess(flags), FILE_SHARE_READ, NULL, FFileOpenFlags::ToDisposition(flags), FILE_ATTRIBUTE_NORMAL, NULL);

  if(m_Handle == INVALID_HANDLE_VALUE)
  {
    LogError("File couldn't be created: %s", file.c_str());
    return false;
  }

  LogDebug("File [%s] has been opened with flags [%s]", file.c_str(), FFileOpenFlags::ToString(flags).c_str());

  return true;
};

int cFileWin32::Write(void *data, DWORD length)
{
  if(!IsOpenForWrite() || !data)
    return -1;

  // Legal NOP
  if(!length)
    return 0;

  DWORD total_done = 0;

  for(; total_done < length;)
  {
    DWORD done = 0;

    BOOL success = WriteFile(m_Handle, (char*)data + total_done, length - total_done, &done, NULL);

    if(success != TRUE || !done)
    {
      LogError("Couldn't write file [%s]!", m_Path.c_str());
      return total_done;
    }

    total_done += done;
  }

  return total_done;
};

int cFileWin32::Read(void *dest, int length)
{
  if(!IsOpenForRead() || !dest)
    return -1;

  int total_done = 0;

  for(; total_done < length;)
  {
    DWORD done = 0;

    BOOL success = ReadFile(m_Handle, (char*)dest + total_done, length - total_done, &done, NULL);

    if(success != TRUE || !done)
    {
      LogError("Couldn't read file [%s]!", m_Path.c_str());
      return total_done;
    }

    total_done += done;
  }

  return total_done;
};

bool cFileWin32::Seek(int offset, EFileSeekMethod::TYPE method)
{
  if(!IsOpen())
    return false;

  DWORD rv = SetFilePointer(m_Handle, offset, NULL, EFileSeekMethod::ToMoveMethod(method));

  return rv != INVALID_SET_FILE_POINTER;
};

DWORD cFileWin32::GetSize() const
{
  if(!IsOpen())
    return 0;

  return m_Size;
};

DWORD cFileWin32::GetOpenFlags() const
{
  return m_Flags;
};

bool cFileWin32::IsOpen() const
{
  return m_Handle != INVALID_HANDLE_VALUE;
};

bool cFileWin32::IsOpenForRead() const
{
  return IsOpen() && (GetOpenFlags() & FFileOpenFlags::Read);
};

bool cFileWin32::IsOpenForWrite() const
{
  return IsOpen() && (GetOpenFlags() & FFileOpenFlags::Write);
};

void cFileWin32::Close()
{
  if(IsOpen())
  {
    CloseHandle(m_Handle);
    Clear();
  }
}

int cFileWin32::GetIndex() const
{
  return m_Index;
};

void cFileWin32::Clear()
{
  m_Handle = INVALID_HANDLE_VALUE;
  m_Flags = 0;
  m_Size = 0;
};

#endif
