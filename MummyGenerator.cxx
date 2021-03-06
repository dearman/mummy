//----------------------------------------------------------------------------
//
//  $Id: MummyGenerator.cxx 2 2007-12-17 18:15:56Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2007-12-17 13:15:56 -0500 (Mon, 17 Dec 2007) $
//  $Revision: 2 $
//
//  Copyright (C) 2006-2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MummyGenerator.h"
#include "MummyLineOrientedTextFileReader.h"
#include "MummyLog.h"
#include "MummySettings.h"

#include "cableClass.h"

#include "gxsys/RegularExpression.hxx"


//----------------------------------------------------------------------------
MummyGenerator::MummyGenerator()
{
  this->Settings = 0;
  this->TargetClass = 0;
  this->HeaderFileReader = 0;
}


//----------------------------------------------------------------------------
MummyGenerator::~MummyGenerator()
{
  if (this->HeaderFileReader)
    {
    delete this->HeaderFileReader;
    this->HeaderFileReader = 0;
    }
}


//----------------------------------------------------------------------------
MummySettings* MummyGenerator::GetSettings()
{
  return this->Settings;
}


//----------------------------------------------------------------------------
void MummyGenerator::SetSettings(MummySettings* settings)
{
  this->Settings = settings;
}


//----------------------------------------------------------------------------
const cable::Class* MummyGenerator::GetTargetClass()
{
  return this->TargetClass;
}


//----------------------------------------------------------------------------
void MummyGenerator::SetTargetClass(const cable::Class *c)
{
  this->TargetClass = c;
}


//----------------------------------------------------------------------------
bool MummyGenerator::FundamentalTypeIsWrappable(const cable::Type*)
{
  return false;
}


//----------------------------------------------------------------------------
bool MummyGenerator::TypeIsWrappable(const cable::Type*)
{
  return false;
}


//----------------------------------------------------------------------------
bool MummyGenerator::FunctionTypeIsWrappable(const cable::FunctionType*)
{
  return false;
}


//----------------------------------------------------------------------------
bool MummyGenerator::MethodIsWrappable(const cable::Method*, const cable::Context::Access&)
{
  return false;
}


//----------------------------------------------------------------------------
bool MummyGenerator::ClassIsWrappable(const cable::Class* c)
{
  MummySettings* settings = this->GetSettings();
  if (settings)
  {
    return settings->ClassIsWrappable(c);
  }

  return false;
}


//----------------------------------------------------------------------------
MummyLineOrientedTextFileReader* MummyGenerator::GetHeaderFileReader(const cable::Class* c)
{
  if (0 == this->HeaderFileReader)
    {
    this->HeaderFileReader = new MummyLineOrientedTextFileReader;

    MummySettings* settings = this->GetSettings();
    if (settings)
      {
      ClassWrappingSettings cws;

      if (settings->FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
        {
        this->HeaderFileReader->SetExcludeMarkedLines(cws.excludeMarkedLines);
        this->HeaderFileReader->SetBeginExcludeRegex(cws.beginExcludeRegex);
        this->HeaderFileReader->SetEndExcludeRegex(cws.endExcludeRegex);
        }
      else
        {
        LogError(me_NoClassWrappingSettings,
          << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
        }
      }

    this->HeaderFileReader->SetFileName(c->GetFile());
    }
  else
    {
    if (this->HeaderFileReader->GetFileName() != c->GetFile())
      {
      LogError(me_InternalError,
        << "Trying to open a different file for HeaderFileReader..." << gxsys_stl::endl
        << "  class: " << GetFullyQualifiedNameForCPlusPlus(c).c_str() << gxsys_stl::endl
        << "  c->GetFile(): " << c->GetFile() << gxsys_stl::endl
        << "  this->HeaderFileReader->GetFileName(): " << this->HeaderFileReader->GetFileName() << gxsys_stl::endl
        );
      }
    }

  return this->HeaderFileReader;
}


//----------------------------------------------------------------------------
void MummyGenerator::EmitMummyVersionComments(gxsys_ios::ostream &os, const char *lineCommentString)
{
  gxsys_stl::string mummy_version(this->GetSettings()->GetMummyVersion());

  if (!lineCommentString)
    {
    lineCommentString = "//";
    }

  Emit(os, lineCommentString);
  Emit(os, "----------------------------------------------------------------------------\n");
  Emit(os, lineCommentString);
  Emit(os, "\n");
  Emit(os, lineCommentString);
  Emit(os, " This file was machine generated by:\n");
  Emit(os, lineCommentString);
  Emit(os, " ");
  Emit(os, mummy_version.c_str());
  Emit(os, "\n");
  Emit(os, lineCommentString);
  Emit(os, "\n");
  Emit(os, lineCommentString);
  Emit(os, " Manual changes to this file may be overwritten by the next build.\n");
  Emit(os, lineCommentString);
  Emit(os, "\n");
}
