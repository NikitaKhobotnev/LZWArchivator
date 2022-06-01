# LZWArchivator
Archiver utility sources using the LZW algorithm

## using
The archiver accepts arguments according to the pattern: -flag argument.
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
             : takes no argument, shows information about the archiver

Use case:
	--source "C:\Program Files\Windows NT\TableTextService\TableTextService.dll" --destin "C:\Example\" --volumes 1 --mode compress
	--source "TotalCommanderPortable.ini" --volumes 16 --mode compress
	--source "\test1\archive1.lzwa" --destin "C:\Example2\" --mode decompress
	--source "E:\test1\big.txt" --mode compress


## lzwa-archive format specification

The archive is written from top to bottom: first, meta-information is written, then several pieces of information about volumes, then the compressed information itself as a sequence of bytes

Ьeta-information:
```
+------+----------+------------------------------------------------------+
|bytes |   type   | info                                                 |
+------+----------+------------------------------------------------------+
|0     | integer  | number n [0, 255] - number of characters per line    |
+------+----------+------------------------------------------------------+
|1-n   | string   | file extension string                                |
+------+----------+------------------------------------------------------+
|n+1   | integer  | number x [0, 255] - number of volumes	               |
+------+----------+------------------------------------------------------+
```
information about volumes (There are exactly x such sequences):
```
+------+----------+------------------------------------------------------+
|bytes |   type   | info                                                 |
+------+----------+------------------------------------------------------+
|0-3   | integer  | 32 bit number y - number of members                  |
+------+----------+------------------------------------------------------+
|4-?   | integer  | contains 32-bit numbers with indices of the          |
|      |   array  | offset that occurred                                 |
+------+----------+------------------------------------------------------+
|last  | integer  | index of last element                                |
|      |          | (shift back to 8 bits)                               |
+------+----------+------------------------------------------------------+
```
compressed information:
an array of numbers containing information from the compressed file.
As the numbers increase, their bit depth increases.
At the end, zero bits are added to provide a multiplicity of 8.
```
+------+----------+------------------------------------------------------+
|bytes |   type   | info                                                 |
+------+----------+------------------------------------------------------+
|      |          | array of numbers containing info from the compressed |
|0-?   | integer  | file. As the numbers increase, their bit             |
|      |   array  | depth increases. At the end, null bits are added     |
|      |          | to provide a multiplicity of 8.                      |
+------+----------+------------------------------------------------------+
```
