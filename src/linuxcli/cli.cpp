
#include<string>
#include<iostream>
#include<sstream>
#include <locale>

#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<dirent.h>

#include "core/smart_ptr.hpp"
#include "core/engine.hpp"
#include "core/document.hpp"
#include "core/client.hpp"

#include"Client.h"
#include"WaveFile.h"

#define VOICES_DIR "/usr/local/share/RHVoice/voices"

#define ERROR_PREFIX "RHVoice:error:"
#define WARNING_PREFIX "RHVoice:warning:"

std::string argText;
std::string argInputFile = "-";
std::string argOutputFile;
std::string argSpeaker = "aleksandr";
int argPitch = 50, argRate = 50, argVolume = 50;
bool argWave = 0, argRaw = 0;

void printHelp()
{
  std::cout << 
    "RHVoice: Russian HTS Voice" << std::endl <<
    std::endl <<
    "Usage:" << std::endl <<
    "\tRHVoice [OPTIONS] [--] [TEXT_TO_SPEAK]" << std::endl <<
std::endl <<
    "Valid command line options are:" << std::endl << 
    "-h, --help - print this help screen and exit" << std::endl <<
    "-i FILENAME, --input-file FILENAME - take the text to speak from FILENAME, \'-\' means using stdin (default if TEXT_TO_SPEAK is empty)" << std::endl <<
    "-o FILENAME, --output-file FILENAME - save the output data to FILENAME, if not specified stdout is used" << std::endl <<
    "-w, --wave - add the wave header before generated data (the default, useful only for explicit mentioning this mode)" << std::endl <<
    "--raw - do not add the wave header to generated data, may not be used with --wave(-w)" << std::endl << 
    "-s VOICE, --speaker VOICE - choose VOICE as a name of the speaker, \'help\' or \'list\' values print all available voices" << std::endl <<
    "-r VALUE --rate VALUE - set speech rate to VALUE, VALUE should be in range from 0 to 100 (default is 50)" << std::endl <<
    "-p VALUE --pitch VALUE - set speech pitch to VALUE, VALUE should be in range from 0 to 100 (default is 50)" << std::endl <<
    "-v VALUE --vol VALUE - set speech volume to VALUE, VALUE should be in range from 0 to 100 (default is 50)" << std::endl <<
    std::endl <<
    "All options beginning from the first unknown are treated as a text to speak." << std::endl <<
    " \'--\' may be used as an explicit mark to take all remaining options as TEXT_TO_SPEAK." << std::endl;
}

std::string getOptArg(int argc,
		      char* argv[],
		      int index,
		      const std::string& argFor)
{
  assert(argv);
  if (index >= argc)
    {
      std::cerr << ERROR_PREFIX << "option \'" << argFor << "\' requires a mandatory argument but goes as a last option in the command line" << std::endl;
      exit(EXIT_FAILURE);
    }
  assert(argv[index]);
  const std::string value = argv[index];
  if (value == "--")
    {
      std::cerr << ERROR_PREFIX << "option \'" << argFor << "\' requires a mandatory argument but its value is \'--\' which designates the end of the command line" << std::endl;
      exit(EXIT_FAILURE);
    }
  return value;
}

int getIntArg(const std::string& value, const std::string& argFor) 
{
  if (value.empty())
    {
      std::cerr << ERROR_PREFIX << "option \'" << argFor << "\' requires an integer argument but it is an empty string" << std::endl;
      exit(EXIT_FAILURE);
    }
  std::istringstream ss(value);
  int res;
  if (!(ss >> res))
    {
      std::cerr << ERROR_PREFIX << "value \'" << value << "\' used as an argument for option \'" << argFor << "\' isn't a valid integer number" << std::endl;
      exit(EXIT_FAILURE);
    }
  if (res < 0 || res > 100)
    {
      std::cerr << ERROR_PREFIX << "argument \'" << res << "\' for option \'" << argFor << "\' must be an integer number between 0 and 100" << std::endl;
      exit(EXIT_FAILURE);
    }
  return res;
}

void printSpeakers()
{
  std::vector<std::string> items;
  DIR* dir = opendir(VOICES_DIR);
  if (dir == NULL)
    {
      std::cerr << ERROR_PREFIX << "directory \'" << VOICES_DIR << "\' is inaccessible for reading" << std::endl;
      exit(EXIT_FAILURE);
    }
  struct dirent* ent;
  while ((ent = readdir(dir)) != NULL)
    {
      const std::string name(ent->d_name);
      if (name != "." && name != "..")
	items.push_back(name);
    }
  closedir(dir);
  std::sort(items.begin(), items.end());
  for(size_t i = 0;i < items.size();++i)
    std::cout << items[i] << std::endl;
  exit(EXIT_SUCCESS);
}

void parseCmdLIne(int argc, char* argv[])
{
  assert(argv);
  for(int i = 1;i < argc;++i)
    {
      assert(argv[i]);
      assert(i + 1 >= argc || argv[i + 1] != NULL);
      const std::string opt = argv[i];
      //--help;
      if (opt == "-h" || opt == "--help")
	{
	  printHelp();
	  exit(EXIT_SUCCESS);
	}
      //--wave;
      if (opt == "-w" || opt == "--wave")
	{
	  argWave = 1;
	  continue;
	}
      //--raw;
      if (opt == "--raw")
	{
	  argRaw = 1;
	  continue;
	}
      //--rate;
      if (opt == "-r" || opt == "--rate")
	{
	  argRate = getIntArg(getOptArg(argc, argv, i + 1, opt), opt);
	  ++i;
	  continue;
	}
      //--pitch;
      if (opt == "-p" || opt == "--pitch")
	{
	  argPitch = getIntArg(getOptArg(argc, argv, i + 1, opt), opt);
	  ++i;
	  continue;
	}
      //--vol;
      if (opt == "-v" || opt == "--vol")
	{
	  argVolume = getIntArg(getOptArg(argc, argv, i + 1, opt), opt);
	  ++i;
	  continue;
	}
      //--speaker;
      if (opt == "-s" || opt == "--speaker")
	{
	  argSpeaker = getOptArg(argc, argv, i + 1, opt);
	  if (argSpeaker == "help" || argSpeaker == "list")
	    printSpeakers();
	  ++i;
	  continue;
	}
      //--input-file;
      if (opt == "-i" || opt == "--input-file")
	{
	  argInputFile = getOptArg(argc, argv, i + 1, opt);
	  ++i;
	  continue;
	}
      //--output-file;
      if (opt == "-o" || opt == "--output-file")
	{
	  argOutputFile = getOptArg(argc, argv, i + 1, opt);
	  ++i;
	  continue;
	}
      //Beginning of the text to speak;
      if (opt == "--")
	++i;
      while (i < argc)
	{
	  assert(argv[i]);
	  argText += argv[i++];
	  argText += " ";
	}
      if (!argText.empty())
	argText.resize(argText.length() - 1);
      break;
    }
  if (argSpeaker.empty())
    {
      std::cerr << ERROR_PREFIX << "speaker name may not be empty (use \'--speaker\' or \'-s\' command line options)" << std::endl;
      exit(EXIT_FAILURE);
    }
  if (argText.empty())
    {
      std::cerr << ERROR_PREFIX << "no text to speak" << std::endl;
      exit(EXIT_FAILURE);
    }
  if (argWave && argRaw)
    {
      std::cerr << ERROR_PREFIX << "options \'--wave(-w)\' and \'--raw\' may not be used simultaneously" << std::endl;
	exit(EXIT_FAILURE);
    }
  if (!argRaw)
    argWave = 1;
}

int run(int argc, char* argv[])
{
  RHVoice::smart_ptr<RHVoice::engine> engine = RHVoice::engine::create();
  WaveFile waveFile;
  Client client(waveFile);
  const std::string& text = argText;
  RHVoice::voice_profile speakers = engine->create_voice_profile(argSpeaker);

  if (!argOutputFile.empty() && argOutputFile != "-")
    {
      std::string errMsg;
  if (!waveFile.openOnDisk(argOutputFile, errMsg))
    {
      std::cerr << ERROR_PREFIX << errMsg << std::endl; 
      return EXIT_FAILURE;
    }
    } else
    waveFile.openAsStdOut();
  if (argWave)
    waveFile.writeHeader(16000);

  std::auto_ptr<RHVoice::document> doc = RHVoice::document::create_from_plain_text(engine, text.begin(), text.end(), RHVoice::content_text, speakers);
  doc->speech_settings.absolute.rate = (float)argRate / 100;
  doc->speech_settings.absolute.pitch = (float)argPitch / 100;
  doc->speech_settings.absolute.volume = (float)argVolume / 100;
  doc->set_owner(client);
            doc->synthesize();

	    waveFile.close();
  return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
  std::locale::global(std::locale(""));
  parseCmdLIne(argc, argv);
  try {
  return run(argc, argv);
  }
  catch (const std::exception& e)
    {
      std::cerr << ERROR_PREFIX << e.what() << std::endl;
    }
}

