# -*- coding:utf-8 -*-
#-----------------------------------------------------------------------------
#  Copyright (c) 2015, ROOT Team.
#  Authors: Omar Zapata <Omar.Zapata@cern.ch> http://oproject.org
#  website: http://oproject.org/ROOT+Jupyter+Kernel (information only for ROOT kernel)
#  Distributed under the terms of the Modified LGPLv3 License.
#
#  The full license is in the file COPYING.rst, distributed with this software.
#-----------------------------------------------------------------------------
from ROOT import gInterpreter


#required c header for i/o
CHeaders =  'extern "C"\n'
CHeaders += '{\n'
CHeaders += '  #include<string.h>\n'
CHeaders += '  #include <stdio.h>\n'
CHeaders += '  #include <stdlib.h>\n'
CHeaders += '  #include <unistd.h>\n'
CHeaders += '  #include<fcntl.h>\n'
CHeaders += '}\n'


#required class to capture i/o
#NOTE: this class required a system to flush in a fork
#if pipe is 1M 1024*1024 size then hung up


CPPIOClass ='class JuPyROOTExecutorHandler{\n'
CPPIOClass +='private:\n'
CPPIOClass +='    bool capturing=false;\n'
CPPIOClass +='    long buffer_size=1048575;//1M\n'
CPPIOClass +='    char *stdout_buff=NULL;\n'
CPPIOClass +='    char *stderr_buff=NULL;\n'
CPPIOClass +='    char *saved_stdout_buff=NULL;\n'
CPPIOClass +='    char *saved_stderr_buff=NULL;\n'
CPPIOClass +='    std::string stdout_out="";\n'
CPPIOClass +='    std::string stderr_out="";\n'
CPPIOClass +='public:   \n'
CPPIOClass +='  JuPyROOTExecutorHandler(long bsize=1048575)\n'
CPPIOClass +='  {\n'
CPPIOClass +='      buffer_size=bsize;\n'
CPPIOClass +='      stdout_buff=new char[buffer_size];\n'
CPPIOClass +='      stderr_buff=new char[buffer_size];\n'
CPPIOClass +='      saved_stdout_buff=new char[buffer_size];\n'
CPPIOClass +='      saved_stderr_buff=new char[buffer_size];\n'
CPPIOClass +='  }\n'
CPPIOClass +='  ~JuPyROOTExecutorHandler()\n'
CPPIOClass +='  {\n'
CPPIOClass +='          if(saved_stdout_buff!=NULL)\n'
CPPIOClass +='          {\n'
CPPIOClass +='              free(saved_stdout_buff);\n'
CPPIOClass +='              saved_stdout_buff=NULL;\n'
CPPIOClass +='          }\n'
CPPIOClass +='          if(stdout_buff!=NULL)\n'
CPPIOClass +='          {\n'
CPPIOClass +='              free(stdout_buff);\n'
CPPIOClass +='              stdout_buff=NULL; \n'
CPPIOClass +='          }\n'
CPPIOClass +='          if(saved_stderr_buff!=NULL)\n'
CPPIOClass +='          {\n'
CPPIOClass +='              free(saved_stderr_buff);\n'
CPPIOClass +='              saved_stderr_buff=NULL;\n'
CPPIOClass +='          }\n'
CPPIOClass +='          if(stderr_buff!=NULL)\n'
CPPIOClass +='          {\n'
CPPIOClass +='              free(stderr_buff);\n'
CPPIOClass +='              stderr_buff=NULL;\n'          
CPPIOClass +='          } \n'
CPPIOClass +='  }\n'
CPPIOClass +='  void InitCapture()\n'
CPPIOClass +='  {\n'
CPPIOClass +='      if(!capturing)\n'
CPPIOClass +='      {\n'
CPPIOClass +='          fflush(stdout);\n'
CPPIOClass +='          setvbuf(stdout,stdout_buff,_IOFBF,buffer_size);//stdout uses your buffer\n'
CPPIOClass +='          fflush(stderr);\n'
CPPIOClass +='          setvbuf(stderr,stderr_buff,_IOFBF,buffer_size);//stdout uses your buffer\n'
CPPIOClass +='          capturing=true;\n'
CPPIOClass +='      }\n'
CPPIOClass +='  }\n'
CPPIOClass +='  void EndCapture()\n'
CPPIOClass +='  {\n'
CPPIOClass +='     if(capturing)\n'
CPPIOClass +='     {\n'
CPPIOClass +='          fflush(stdout);\n'
CPPIOClass +='          strncpy(saved_stdout_buff,stdout_buff,buffer_size);\n'
CPPIOClass +='          stdout_out=saved_stdout_buff;\n'
CPPIOClass +='          setbuf(stdout,NULL);\n'
CPPIOClass +='          fflush(stderr);\n'
CPPIOClass +='          strncpy(saved_stderr_buff,stderr_buff,buffer_size);\n'
CPPIOClass +='          stderr_out=saved_stderr_buff;\n'
CPPIOClass +='          setbuf(stderr,NULL);\n'
CPPIOClass +='          memset(stderr_buff, 0, buffer_size);\n'
CPPIOClass +='          memset(stdout_buff, 0, buffer_size);\n'
CPPIOClass +='          capturing=false;\n'
CPPIOClass +='     }\n'
CPPIOClass +='  }\n'
CPPIOClass +='  std::string getStdout(){return stdout_out;}\n'
CPPIOClass +='  std::string getStderr(){return stderr_out;}\n'
#CPPIOClass +='  void clear(){\n'
#CPPIOClass +='          stdout_out="";\n'
#CPPIOClass +='          stderr_out="";\n'
#CPPIOClass +='    }\n'
CPPIOClass +='};\n'





#function to execute capturing segfault
CPPIOFunctions ='Bool_t JuPyROOTExecutor(TString code)'
CPPIOFunctions +='{'
CPPIOFunctions +='  Bool_t status=kFALSE;'
CPPIOFunctions +='  TRY {'
CPPIOFunctions +='    if(gInterpreter->ProcessLine(code.Data()))'
CPPIOFunctions +='    {'
CPPIOFunctions +='      status=kTRUE;'
CPPIOFunctions +='    }'
CPPIOFunctions +='  } CATCH(excode) {'
CPPIOFunctions +='    status=kTRUE;'
CPPIOFunctions +='  } ENDTRY;'
CPPIOFunctions +='  return status;'
CPPIOFunctions +='}'


#function to declare capturing segfault
CPPIOFunctions +='Bool_t JuPyROOTDeclarer(TString code)'
CPPIOFunctions +='{'
CPPIOFunctions +='  Bool_t status=kFALSE;'
CPPIOFunctions +='  TRY {'
CPPIOFunctions +='    if(gInterpreter->Declare(code.Data()))'
CPPIOFunctions +='    {'
CPPIOFunctions +='      status=kTRUE;'
CPPIOFunctions +='    }'
CPPIOFunctions +='  } CATCH(excode) {'
CPPIOFunctions +='    status=kTRUE;'
CPPIOFunctions +='  } ENDTRY;'
CPPIOFunctions +='  return status;'
CPPIOFunctions +='}'

def _LoadHeaders():
    gInterpreter.ProcessLine("#include<TRint.h>")
    gInterpreter.ProcessLine("#include<TApplication.h>")
    gInterpreter.ProcessLine("#include<TException.h>")
    gInterpreter.ProcessLine("#include<TInterpreter.h>")
    gInterpreter.ProcessLine("#include <TROOT.h>")
    gInterpreter.ProcessLine("#include<string>")
    gInterpreter.ProcessLine("#include<sstream>")
    gInterpreter.ProcessLine("#include<iostream>")
    gInterpreter.ProcessLine("#include<fstream>")    
    gInterpreter.ProcessLine(CHeaders)

def _LoadClass():
    gInterpreter.Declare(CPPIOClass)


def _LoadFunctions():
    gInterpreter.Declare(CPPIOFunctions)
    
def LoadHandlers():
    _LoadHeaders()
    _LoadClass()
    _LoadFunctions()
