/*
 *       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Denmark A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *  Tool for adding a static, dynamic and calibration field in a flash file.
 *  It is also possible to sign the flash file with a certificate.
 *
 * A list of options is displayed if run without arguments.
 *
 * Usage Example:
 *  ./FlsTool --inject -i psi.fls -o psi.fls.fls -n nvm/ -c cert/
 *
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/FlsTool/src/main.cpp $
 *  $CC-Version: .../dhkristx_140314_v0/2 $
 *  $Date:       2014-04-30    10:42:26 UTC $
 *   Resonsible:
 *   Comment:
 *
 */

#include "version.h"

#include "FlsFile.h"
#include "PrgOptions.h"
#include "Convert.h"       //< IntelHexFile
#include "FileIO.h"
#include "OS.h"

#include <cstdio>

#include <iostream>
#include <string>
#include <vector>

#ifndef WIN32
  #include <errno.h>
  #include <fcntl.h>
  #include <sys/wait.h>

  #if defined(DEBUG) && !defined(ANDROID)
    #include <execinfo.h>

    void my_terminate(void);
    static void print_trace(string file = "");

    // void setup_signals();
    void signal_handler (int signo, siginfo_t *si, void *data);

    static const bool SET_TERMINATE = std::set_terminate(my_terminate);
    //static const bool SET_UNEXPENDED = std::set_unexpected( my_terminate );
  #endif
#endif

using namespace std;
using namespace ipa;
using namespace ipa::fls;

//---------------------------------------------------------------------------

int prompt = false;
int verbose = 1;

#define OVERWRITE    true
#define NO_OVERWRITE false

//---------------------------------------------------------------------------

void set_tool_version(FlsFile& file, bool overwrite = false)
{
  if (overwrite || file.tool_version.empty())
    file.tool_version = "FlsTool_v."+string(VERSION_STRING);
}


/**
 * Verifies an input file path.
 *
 * First it checks if the output path is defined.
 * If path is a file it will remove it in case overwrite is true (default false).
 * If path contains a directory that is non-existing the directory is created.
 *
 * @param path       The path to verify
 * @param directory  Set to true if checking a directory path
 *
 * @return   True if verified, false if not
 */
static bool VerifyInputPath(string path, bool directory = false)
{
  if (path.empty())
  {
    throw_exception("Please specify an input path", false);
  }
  else if (directory && !FileSystem_isdir(path))
  {
    throw_exception("Input directory["+path+"] does not exist!", false);
  }
  else if (!FileSystem_isfile(path))
  {
    throw_exception("Input file["+path+"] does not exist!", false);
  }
  return true;
}

/**
 * Verifies an output file path (Not directory path).
 *
 * First it checks if the output path is defined.
 * If path is a file it will remove it in case overwrite is true (default false).
 * If path contains a directory that is non-existing the directory is created.
 *
 * @param path       The path to verify
 * @param overwrite  Overwrite in case the file exists (default false)
 *
 * @return   True if verified, false if not
 */
static bool VerifyOutputPath(string path, bool overwrite = false)
{
  if (path.empty())
  {
    throw_exception("Please specify an output path", false);
  }
  else if (FileSystem_isfile(path))
  {
    if (!overwrite)
      throw_exception("Output path "+ path +" exists!\n\t Use --replace to overwrite.", false);

    if (overwrite && !FileSystem_remove(path.c_str()))
      throw_exception("Error deleting "+ path, false);
  }
  else if (path_is_relative(path.c_str()))
  {
    string dir = fdir(path);
    if (!FileSystem_isdir(dir))
    {
      if (!FileSystem_mkdir(dir))
        throw_exception("Could not create output directory["+dir+"]", false);
    }
  }
  return true;
}

//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  size_t i;
  int replace = false;
  int ret = 0;
  string app = fname(argv[0]);
  string NvmPath, CertPath, SecPath;
  //vector<string> in;
  string in, out;

  vector<string> input_files;
  string prg, psi, ebl, xml, zip, script, meta, tag;

  #ifdef INCLUDE_FLS3
    int ForceFls2 = false; //< Force the deprecated Fls2 format
  #else
    int ForceFls2 = true;
  #endif

  #if defined(DEBUG) && !defined(ANDROID) && !defined(WIN32)
    struct sigaction sa, osa;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sa.sa_flags = SA_ONSTACK | SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    //sigaction(SIGABRT, &sa, &osa);
    sigaction(SIGSEGV, &sa, &osa);
  #endif

  PrgOptions opt(37);

  opt.addHeader(string("This tool can do several different operations of FLS files.\n")
                      +"Use the 'Action' option to select to required operation.");

  //opt.addSeparator("\n ***************************************************** \n");
  opt.add("Actions:");
  opt.add("pack,p",                          "Packing multiple FLS files into one");
  opt.add("inject,i",                        "Inject NVM, Certificates or Security into FLS file");
  opt.add("extract,x",                       "Extract all image parts from the FLS file(s)" );
  opt.add("extract-fls",                     "Extract embedded files from the FLS file(s)" );
  opt.add("extract-prg",                     "Extract PRG file" );
  opt.add("to-bin,b",                        "Convert a single Hex file to binary file" );
  opt.add("hex-to-fls",                      "Create an Fls from a Prg file");
  opt.add("sign",                            "Formerly known as FlsSign");
#ifdef INCLUDE_FLS3
  opt.add("to-fls2",                         "Convert an Fls3 file to Fls2 format" );
  opt.add("to-fls3",                         "Convert an Fls2 file to Fls3 format" );
#else
  opt.add("to-fls2",                         "Convert the experimental Fls3 file to Fls2 format" );
  opt.add("to-fls3",                         "Convert an Fls2 file to the experimental Fls3 format" );
#endif
  opt.add("dump,d",                          "Dump the meta data of an FLS file.");
  opt.add("sec-pack",                        "Dump all SecPack data of an FLS file.");

  opt.add("HexToFls options:");
  opt.add("prg=s",         &prg,             "Choose a PRG file to create the Fls from");
  opt.add("psi=s",         &psi,             "Add a PSI to the Fls file (replaces if '-r' option)");
  opt.add("ebl=s",         &ebl,             "Add an EBL to the Fls file (replaces if '-r' option)");

  opt.add("meta=s",        &meta,            "Inject any meta file to the Fls file (Equal to --version or -v in HexToFls)");
  opt.add("xml=s",         &xml,             "Add an XML file to the Fls file (replaces if '-r' option)");
  opt.add("zip=s",         &zip,             "Add a ZIP file to the Fls file (replaces if '-r' option)");
  opt.add("script=s",      &script,          "Add a Script file to the Fls file (replaces if '-r' option)");
  opt.add("tag=s",         &tag,             "Specifies the memory region tag to insert the input file (replaces if '-r' option)");

#if 0
  opt.add("remove-psi",                      "Removes the PSI currently in the fls file");
  opt.add("remove-ebl",                      "Removes the EBL currently in the fls file");
  opt.add("remove-tag=s",  &tag,             "Removes all sections with tag ");
#endif

  opt.add("Inject options:");
  opt.add("nvm-path,n=s",  &NvmPath,         "Path to the NVM input files");
  //opt.add("cert-path,c=s", &CertPath,        "Path to the certificate.bin file");
  //opt.add("sec-path,s=s",  &SecPath,         "Path to the sec.bin file");

  opt.add("Generic Options:");
  opt.add("output,o=s",    &out,             "Output path");
#ifdef INCLUDE_FLS3
  opt.add("force-fls2",    &ForceFls2, true, "Force the use of the deprecated Fls2 format.");
#endif
  opt.add("replace,r",     &replace, true,  "Defaults to replace when trying to add a section which is already existing");
  opt.add("verbose,v=i",   &verbose, 1,      "Set verbosity");
  opt.add("prompt",        &prompt,  true,  "Prompt before quitting");
  opt.add("version",                         "Show the version of this tool");
  opt.add("help,h",                          "Show command line help");
  //opt.addTail("\n ***************************************************** \n");

  try {
    opt.parse(argc, argv);
  }
  catch (const exception& e)
  {
    opt.showDescription();
    #ifndef ANDROID
      cerr << endl << " Parse error> " << e.what() << endl;
    #endif
    #ifdef _WIN32
      if (argc < 2)
      {
        cout << "Press any key to quit. "; cout.flush();
        getchar();
      }
    #endif
    exit(1);
  }

  if (verbose && !opt.is_set("dump") && !opt.is_set("sec-pack"))
    cout << app << " v." << VERSION_STRING << endl;

  if (opt.is_set("version"))
    exit(0);

  if (opt.is_set("help"))
  {
    cout << " This product includes software developed by the OpenSSL Project"
         << endl << " for use in the OpenSSL Toolkit (http://www.openssl.org/)" << endl;

    char* last_arg = argv[argc-1];
    if (last_arg[0] == '-')
      opt.showDescription();
    else
    {
      string arg = last_arg;
      cout << endl;
      if (arg.compare("pack") == 0)
        cout << "Action 'Pack': Packing multiple FLS files into one."
             << endl << endl
             << " Usage: " << app << " --pack [-o <output_file>] input_file1 ...";
      else
        if (arg.compare("inject") == 0)
          cout << "Action 'Inject': Used to inject data into FlashLess system FLS file."
               << endl << endl
               << " Usage: " << app << " --inject input_file -o <output_file> "
               << "-n <NVM path> [ -x -s <Sec path> -c <Cert path> ]";
      else
        if (arg.compare("extract") == 0)
          cout << "Action 'Extract': Save all image parts from FLS file(s) to separate files."
               << endl << endl
               << " Usage: " << app << " --extract input file(s) [-o <output folder>]";
      else
        if (arg.compare("extract-fls") == 0)
          cout << "Action 'extract-fls': Extract embedded files from FLS file(s)."
               << endl << endl
               << " Usage: " << app << " --extract-fls --tag tag input file(s) [-o <output folder>]";
      else
        if (arg.compare("extract-prg") == 0)
          cout << "Action 'extract-prg': Extract PRG part from FLS file(s)."
               << endl << endl
               << " Usage: " << app << " --extract-prg input file(s) [-o <output folder>]";
      else
        if (arg.compare("to-bin") == 0)
          cout << "Action 'to-bin': Convert a file in Intel hex format to binary format."
               << endl << endl
               << " Usage: " << app << " --to-bin input file [-o <output file>]";
      else
        if (arg.compare("hex-to-fls") == 0)
          cout << "Action 'hex-to-fls': Create FLS file via PRG & input files."
               << endl << endl
               << " Usage: " << app << " --prg <PRG file> -o <output_file> input_file1 ...";
      else
        if (arg.compare("sign") == 0)
          cout << "Action 'sign': Signing Fls files"
               << endl << endl
               << " Usage: " << app << " --sign !!!!! SOMETHING MORE !!!!";
#ifdef INCLUDE_FLS3
      else
        if (arg.compare("to-fls2") == 0)
          cout << "Action 'ToFls2': Convert an Fls2 file to Fls3 format"
               << endl << endl
               << " Usage: " << app << " --to-fls2 input_fls -o output_fls";
      else
        if (arg.compare("to-fls3") == 0)
          cout << "Action 'ToFls3': Convert an Fls3 file to Fls2 format"
               << endl << endl
               << " Usage: " << app << " --to-fls3 input_fls -o output_fls";
#endif
      else
        if (arg.compare("dump") == 0)
          cout << "Action 'dump': Dump the meta data of an Fls or Prg file"
               << endl << endl
               << " Usage: " << app << " --dump input_fls";
      else
        opt.showUsage(last_arg);

      cout << endl << endl;
    }
    exit(0);
  }

  // Non-option arguments are considered input files
  vector<const char*> args = opt.getFiles();
  if (args.size() == 0)
  {
    opt.showDescription();
    cerr << endl << "  Please specify an input file" << endl << endl;
    return -1;
  }

#ifndef DEBUG
  try
  {
#endif
    if (opt.is_set("hex-to-fls") and prg.empty())
      throw_exception("PRG file missing!", 1);

     //< All input files are considered HEX
    for (i=0; i < args.size(); ++i)
      input_files.push_back(args[i]);

    /**
     *  HexToFls functionality - Injecting PSI, EBL, & Code via iHex files
     *
     *   Pack functionality also here.
     */
    if (opt.is_set("pack") || opt.is_set("prg") || opt.is_set("sign") || opt.is_set("zip"))
    {
      // We need to load a MemoryMap via either PRG or an InputFlsFile
      if (!prg.empty())
      {
        in = prg;
      }
      else
      {
        for (size_t i=0; i < input_files.size(); ++i)
        {
          string type = detect_file_type(input_files[i]);
          if (type.compare(0,3,"Fls") == 0)
          {
            in = input_files[i];
            input_files.erase(input_files.begin()+i);
            break;
          }
        }
      }

      if (!VerifyInputPath(in) || (!out.empty() && !VerifyOutputPath(out, replace)))
        return -1;

      FlsFile file(in, verbose);
      string output = fname(out);
      set_tool_version(file, NO_OVERWRITE);

      if (!psi.empty())
        file.append_boot_file(psi, MEMORY_CLASS_PSI);

      if (!ebl.empty())
        file.append_boot_file(ebl, MEMORY_CLASS_EBL);

      if (!xml.empty())
        file.inject_meta_file(xml, output+"_inj.xml", replace);

      if (!zip.empty())
        file.inject_meta_file(zip, fname(zip), replace);

      if (input_files.size() > 0)
        file.append_list(tag, input_files, output);

      if (!script.empty())
        file.run_script(script);

      if (!meta.empty())
        file.inject_meta_file(meta, output+"_inj_version.txt", replace);

      file.version = (ForceFls2) ? 2 : 3;
      file.save(out, replace);

      // NOTE: args are cleared so next loop is avoided.
      args.clear();
    }

    /* Apply action(s) on all input files */
    for (i=0; i < args.size(); ++i)
    {
      in = args[i];

      if (!VerifyInputPath(in) || (!out.empty() && !VerifyOutputPath(out, replace)))
        return -1;

      if (opt.is_set("dump"))
      {
        FlsFile(in, verbose-1).metadata();
      }
      else if (opt.is_set("sec-pack"))
      {
        FlsFile(in, verbose-1).dump_sec_pack();
      }
      else if (opt.is_set("to-bin"))
      {
        cout << " [HexToBin] " << in << " => " << out << endl;

        IntelHexFile iHexFile(in, verbose);
        iHexFile.save(out, replace);
      }
      else
      {
        FlsFile file(in, verbose);
        // file.set_verbose(verbose);
        set_tool_version(file, NO_OVERWRITE);

        // Apply action
        if (opt.is_set("inject"))
        {
          file.inject_nvm(NvmPath);
        }
        else if (opt.is_set("extract-prg"))
        {
          file.clear_files();
        }
        else if (opt.is_set("extract-fls"))
        {
          FlsFile tmp_file(file);
          file.clear_files();
          file.import_tag(tmp_file, tag, in);
        }
        else if (opt.is_set("to-fls2") || opt.is_set("to-fls3"))
        {
          ForceFls2 = opt.is_set("to-fls2");
        }
        else if (!opt.is_set("extract"))
        {
          opt.showDescription();
          cerr << endl << "  Please specify an action" << endl << endl;
          return ret;
        }

        // Save/Extract
        if (opt.is_set("extract"))
        {
          file.extract(out);
        }
        else
        {
          file.version = (ForceFls2) ? 2 : 3;
          file.save(out, replace);
        }
      }
    }

#ifndef DEBUG
  }
  catch (const exception& e)
  {
    #ifndef __EXCEPTIONS
      myexception& e = chatched_exceptions.front();
    #endif

    cerr << endl << " Error> " << e.what() << endl << endl;
    #ifdef _WIN32
      if (prompt)
      {
        cout << "Press any key to quit.";
        getchar();
//    #else
//      print_trace("StackTrace.dbg");
      }
    #endif
    exit(1);
  }
#endif

  /* Show description on error */
  if (ret < 0)
    opt.showDescription();

  if (prompt)
  {
    cout << "Press any key to quit." << endl;
    getchar();
  }

  // cout << " bye bye " << endl;
  return ret;
}

//---------------------------------------------------------------------------
#if defined(DEBUG) && !defined(ANDROID)

/**
 * Some signals are handled by this function (SIGFAULT)
 */
void signal_handler (int signo, siginfo_t *si, void *data)
{
  cerr << endl << "Segmentation fault!" << endl;
  print_trace();
  exit(signo);
}

/**
 *	Dump the stack trace via GDB
 *
 * @param file   Define a file to dump to (Default: stdout)
 */
static void print_trace(string file)
{
  char pid_buf[30];
  sprintf(pid_buf, "%d", getpid());
  char name_buf[512];
  name_buf[readlink("/proc/self/exe", name_buf, 511)]=0;
  int child_pid = fork();
  if (!child_pid)
  {
    if (!file.empty())
    {
      int fd = open(file.c_str(),O_CREAT | O_RDWR , 00777 );
      dup2(fd, 1); // redirect output to file
      close(fd);
    }
    else
    {
      dup2(2, 1); // redirect output to stderr
    }

    cout << endl << "------------ Stack Trace (" << pid_buf << ") ------------" << endl;

    // For more debug change "bt" to "bt full"
    execlp("gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt", name_buf, pid_buf, NULL);
    abort();
  } else {
    waitpid(child_pid,NULL,0);
    if (!file.empty())
      cerr << endl << " Stack Trace saved in " << file << endl;
  }
}

/**
 *	Catching exceptions & Dumping stack trace
 */
void my_terminate(void)
{
//  string trace_file = "stacktrace.dbg";
  static bool tried_throw = false;

  try {
      // try once to re-throw currently active exception
      if (!tried_throw++) throw;
  }
  catch (const std::exception &e) {
      cerr << endl << "  Error> " << e.what() << endl;
  }
  catch (...) {
      cerr << endl << "  Unknown/Unhandled Exception." << endl;
  }

  print_trace();

  abort();
}
#endif
