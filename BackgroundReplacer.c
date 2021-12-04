#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>


int count;
int errorCount;


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("program must take exactly two arguments");
        return 1;
    }
    if (strcmpi(argv[2], "-n") != 0 && strcmpi(argv[2], "-a") != 0)
    {
        printf("invalid second argument, valid arguments are: -n, -a\n");
        return 1;
    }
    FILE* img = fopen(argv[1], "rb");
    if (img == NULL)
    {
        fclose(img);
        printf("invalid file path");
        return 1;
    }
    else 
    {
        char *fileExtension = strrchr(argv[1], *".");
        if (strcmp(fileExtension, ".png") != 0 && strcmp(fileExtension, ".jpg") != 0)
        {   
            fclose(img);
            printf("argument must be a path to a .png or .jpg file");
            return 1;
        }
    }
    fclose(img);

    FILE *img_ptr = fopen(argv[1], "rb");
    fseek(img_ptr, 0, SEEK_END);
    long imgSize = ftell(img_ptr);
    
    if (imgSize >= 250000)
    {
        printf("**WARNING**\nThe image you supplied is over 250 kb large, this might eat your disk space, do you want to continue? (Y/N): ");
        char userInput[5];
        fgets(userInput, sizeof(userInput), stdin);
        if (strcmpi(userInput, "n\n") == 0)
        {
            fclose(img_ptr);
            return 1;
        }
    }
    fclose(img_ptr);

    char cwd[255];
    getcwd(cwd, sizeof(cwd));   
    char *osu = strstr(cwd, "osu!");

    if (osu)
    {
        struct dirent *dirent;
        DIR *dir = opendir("Songs");

        if (dir == NULL)
        {
            closedir(dir);
            printf("failed to open songs directory, try to run as administrator\n");
            return 1;
        }

        FILE *textFile = fopen("Songs\\replaced_bg_list.txt", "a");
        fclose(textFile);
        
        if (strcmpi(argv[2], "-a") == 0)
        {
            FILE *textFile = fopen("Songs\\replaced_bg_list.txt", "w");
            fprintf(textFile, "");
            fclose(textFile);
        }

        while ((dirent = readdir(dir)) != NULL)
        {   
            if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
            {
                continue;
            }

            char path[255];
            snprintf(path, sizeof(path), "%s%s", "Songs\\", dirent->d_name);

            FILE *textFile = fopen("Songs\\replaced_bg_list.txt", "r");
            char readLine[255];

            int flag = 0;

            if (strcmp(argv[2], "-n") == 0)
            {
                while (fgets(readLine, sizeof(readLine), textFile)) // such a shitty solution
                {
                    readLine[strcspn(readLine, "\n")] = 0;
                    if (strcmp(readLine, path) == 0)
                    {
                        ++flag;
                        break;
                    }
                }
            }
            fclose(textFile);
                
            if (flag == 0)
            {
                char *fileExtension = strrchr(dirent->d_name, *".");
                if (fileExtension == NULL)
                {
                    struct dirent *dirent;
                    DIR *dir = opendir(path);

                    if (dir == NULL)
                    {
                        closedir(dir);
                        ++errorCount;
                        printf("Failed to open: %s\n", path);
                        continue;
                    }

                    FILE *textFile = fopen("Songs\\replaced_bg_list.txt", "a");
                    fprintf(textFile, "%s\n", path);
                    fclose(textFile);

                    while ((dirent = readdir(dir)) != NULL) 
                    {
                        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
                        {
                            continue;
                        }

                        char *fileExtension = strrchr(dirent->d_name, *".");
                        if (fileExtension == NULL)
                        {
                            continue;
                        }
                        
                        if (strcmp(fileExtension, ".png") == 0 || strcmp(fileExtension, ".jpg") == 0 || strcmp(fileExtension, ".jpeg") == 0)
                        {
                            char imgPath[255];
                            snprintf(imgPath, sizeof(imgPath), "%s%s%s", path, "\\", dirent->d_name);
                            
                            FILE *argStream = fopen(argv[1], "r");
                            fseek(argStream, 0, SEEK_END);
                            char byteBuffer[ftell(argStream)];
                            fclose(argStream);

                            FILE *imgArgStream = fopen(argv[1], "rb");
                            fread(byteBuffer, sizeof(byteBuffer), 1, imgArgStream);

                            FILE *imgMapStream = fopen(imgPath, "wb");
                            fwrite(byteBuffer, sizeof(byteBuffer), 1, imgMapStream);

                            fclose(imgArgStream);
                            fclose(imgMapStream);

                            ++count;
                            printf("Replaced: %s\n", imgPath);
                        }
                    }
                    closedir(dir);
                }
            }
        }
        closedir(dir);
    }
    else
    {
        printf("put the program in your osu! folder");
        return 1;
    }
    printf("\n\nFinished replacing %d images, and failed to replace %d images\n", count, errorCount);
    return 0;
}
