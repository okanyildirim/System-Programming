# FUSE in Linux 

Using FUSE, implement a file system that will navigate the provided Turkish postal codes data1. The file system will analyze a CSV file that contains this data and display a hierarchy of the neighborhoods based on the location names and codes. The first two digits of the postal code represent the city and the remaining three digits represent a neighborhood in the city. Every neighborhood is in a district of the city. The format of the CSV file is as follows (separated by tabs): 

CODE NEIGHBORHOOD CITY DISTRICT LATITUDE LONGITUDE

The top-level directory will contain two-directories: 
NAMES and CODES. 

The NAMES directory will display a subdirectory for every city. A city directory will display a subdirectory for every district. A district directory will display a text file for every neighborhood. 
For example, the entry for “Maslak” in the CSV file is: 
34398 Maslak Istanbul Sariyer 41.1083 29.0183 

The path for the file will be: 
NAMES → Istanbul → Sariyer → Maslak.txt. 

The contents of this file will be the data presented as key-value pairs: 
code: 34398 neighborhood: Maslak city: Istanbul district: Sariyer latitude: 41.1083 longitude: 29.0183 
The size of the file will be reported as the length of its contents string. The CODES directory will display a subdirectory for every city code. A code subdirectory will display a text file for every neighborhood code. The contents of these files will be the same as above. 

The path for the same example above will be: CODES → 34 → 34398.txt. 

Any changes to the CSV file will be instantly visible through the FUSE-based file system. The only modifications allowed on the file system will be deleting or renaming neighborhood files, for example deleting the file Maslak.txt or renaming the file 34398.txt as 34469.txt. These operations will update the CSV file accordingly. The file system should be testable using standard shell commands such as ls and cat, or using any file manager. Read operation offsets and lengths should be implemented. For example, the command “od -j 23 -N 9 -a Maslak.txt” should work correctly. 

To read and manipulate the CSV file, you can write the code yourself or you can use any library you prefer. An example library can be found at the following address: http://libcsv.sourceforge.net/
