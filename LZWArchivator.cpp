import<iostream>;
import<string>;
import<vector>;

import Archiver;

void HelpInstruction();
void About();
int main(int argc, char *argv[])
{
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; i++)
        arguments.push_back(std::string(argv[i]));

    if (
        arguments.end() != find_if(arguments.begin(), arguments.end(), [](std::string str)
                                   { return str == "--h" || str == "--help" || str == "-h" || str == "-help"; }))
    {
        HelpInstruction();
        std::string arg1, arg2;
        arguments.clear();
        while (std::cin >> arg1 >> arg2)
        {
            arguments.push_back(arg1);
            arguments.push_back(arg2);
        }
    }
    if (
        arguments.end() != find_if(arguments.begin(), arguments.end(), [](std::string str)
                                   { return str == "--a" || str == "--about" || str == "-a" || str == "-about"; }))
    {
        About();
        arguments.erase(find_if(arguments.begin(), arguments.end(), [](std::string str)
                                { return str == "--a" || str == "--about" || str == "-a" || str == "-about"; }));
    }

    size_t num = 0;
    std::string destin = "", source = "", mode = "";
    for (auto iter = arguments.begin(); iter < arguments.end(); iter += 2)
    {
        if (*iter == "--destin" || *iter == "-destin" || *iter == "--d" || *iter == "-d")
        {
            destin = *(iter + 1);
            continue;
        }
        if (*iter == "--source" || *iter == "-source" || *iter == "--s" || *iter == "-s")
        {
            source = *(iter + 1);
            continue;
        }
        if (*iter == "--mode" || *iter == "-mode" || *iter == "--m" || *iter == "-m")
        {
            mode = *(iter + 1);
            continue;
        }
        if (*iter == "--volumes" || *iter == "-volumes" || *iter == "--v" || *iter == "-v")
        {
            num = (size_t)std::stoi(*(iter + 1));
            continue;
        }
    }

    LZWArchivator *_archivator;

    if (destin == "" && num == 0)
        _archivator = new LZWArchivator(source);
    else if (destin == "")
        _archivator = new LZWArchivator(source, num);
    else if (num == 0)
        _archivator = new LZWArchivator(source, destin);
    else
        _archivator = new LZWArchivator(source, destin, num);

    if (mode == "compress" || mode == "c")
        _archivator->Compress();
    if (mode == "decompress" || mode == "d")
        _archivator->Decompress();

    delete _archivator;

    return 0;
}

//выводит инструкции
void HelpInstruction()
{
    const std::string _LittleAbout = R"(This archiver is built on the classic Linux philosophy "Do one thing, but do it well"
)";
    const std::string _Help = R"(The archiver accepts arguments according to the pattern: -flag argument.
There are 6 flags in total, 4 of which take arguments:
	--help [-h]			
             : takes no argument, shows instruction (the one you see now)
	--source [-s] [sourcePath]			
             : takes as an argument the relative name of the file on which operations will
               be performed. That is, the name of the file and the path to it from the folder 
               that is currently open in the terminal. If in doubt - specify the full name 
               (starts with "C:/" or something like that)
	--destin [-d] [destinPath]			
             : takes as an argument the path where the result of the program will be put. 
               Optional, if you do not specify the result will be in the same folder as the source file
	--volumes [-v] [numOfVolumes]		
             : takes as an argument the number of volumes into which the file will be 
               divided during archiving. Tip: do not get carried away, a large number 
               of volumes can nullify all compression and greatly slow down the system. 
               In addition, the file will be decompressed in the same number
               of streams into which it was compressed.
	--mod [-m] [compress/decompress]	
             : takes as an argument a command word indicating an operation. 
               "compress" - will compress the file, 
               "decompress" - will decompress accordingly.
	--about [-a]						
             : takes no argument, shows information about the archiver)";
    const std::string _Example = R"(
Use case:
	--source "C:\Program Files\Windows NT\TableTextService\TableTextService.dll" --destin "C:\Example\" --volumes 1 --mode compress
	--source "TotalCommanderPortable.ini" --volumes 16 --mode compress
	--source "\test1\archive1.lzwa" --destin "C:\Example2\" --mode decompress
	--source "E:\test1\big.txt" --mode compress
)";
    const std::string _DoIt = R"(Now enter the arguments again:
)";

    std::cout << _LittleAbout << _Help << _Example << std::endl;
    std::cout << _DoIt << std::endl;
}

//выводит информацию про
void About()
{
    const std::string _LZW =
        R"(                    ___           ___
                   /  /\         /__/\
                  /  /::|       _\_ \:\
  ___     ___    /  /:/:|      /__/\ \:\
 /__/\   /  /\  /  /:/|:|__   _\_ \:\ \:\
 \  \:\ /  /:/ /__/:/ |:| /\ /__/\ \:\ \:\
  \  \:\  /:/  \__\/  |:|/:/ \  \:\ \:\/:/
   \  \:\/:/       |  |:/:/   \  \:\ \::/
    \  \::/        |  |::/     \  \:\/:/
     \__\/         |  |:/       \  \::/
                   |__|/         \__\/
)";
    const std::string _Archiver =
        R"(      ___           ___           ___           ___                                     ___           ___
     /\  \         /\  \         /\__\         /\  \                      ___          /\__\         /\  \
    /::\  \       /::\  \       /:/  /         \:\  \       ___          /\  \        /:/ _/_       /::\  \
   /:/\:\  \     /:/\:\__\     /:/  /           \:\  \     /\__\         \:\  \      /:/ /\__\     /:/\:\__\
  /:/ /::\  \   /:/ /:/  /    /:/  /  ___   ___ /::\  \   /:/__/          \:\  \    /:/ /:/ _/_   /:/ /:/  /
 /:/_/:/\:\__\ /:/_/:/__/___ /:/__/  /\__\ /\  /:/\:\__\ /::\  \      ___  \:\__\  /:/_/:/ /\__\ /:/_/:/__/___
 \:\/:/  \/__/ \:\/:::::/  / \:\  \ /:/  / \:\/:/  \/__/ \/\:\  \__  /\  \ |:|  |  \:\/:/ /:/  / \:\/:::::/  /
  \::/__/       \::/~~/~~~~   \:\  /:/  /   \::/__/       ~~\:\/\__\ \:\  \|:|  |   \::/_/:/  /   \::/~~/~~~~
   \:\  \        \:\~~\        \:\/:/  /     \:\  \          \::/  /  \:\__|:|__|    \:\/:/  /     \:\~~\
    \:\__\        \:\__\        \::/  /       \:\__\         /:/  /    \::::/__/      \::/  /       \:\__\
     \/__/         \/__/         \/__/         \/__/         \/__/      ~~~~           \/__/         \/__/
)";

    const std::string _About = R"(This is a simple archiver (or rather, not an archiver - it cannot archive, only compress), using the LZW algorithm to compress or, conversely, decompress a single file.
When compressed, an archive is created with the file extension .lzwa, when uncompressed, the .lzwa file is converted into a file with the extension that was during compression.
When writing, C++20 was used.
The code is absolutely open.
The sources are on the github (though in a closed repository. Need a link to an open repository).
Archiver into a separate class and library of functions, which makes it relatively easy to transfer its functionality)";

    const std::string _AboutAuthor = R"(The archiver was written as a term paper by Nikita Khobotnev.)";

    std::cout << _LZW << _Archiver << std::endl;
    std::cout << _About << std::endl;
    std::cout << _AboutAuthor << std::endl;
}