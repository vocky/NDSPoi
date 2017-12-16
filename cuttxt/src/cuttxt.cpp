//============================================================================
// Name        : cuttxt.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <map>
#include <vector>
#include <set>

using namespace std;

#define BUF_SIZE 100
#define MAX_STRING_LENGTH 1024

int CreateDir(const char *sPathName)
{
    char DirName[256];
    strcpy(DirName, sPathName);
    int i,len = strlen(DirName);
    if(DirName[len-1] !='/')
    strcat(DirName, "/");

    len = strlen(DirName);

    for(i = 1; i < len; i++)
    {
        if(DirName[i] == '/')
        {
            DirName[i] = 0;
            if(access((const char*)DirName, 0) != 0)
            {
              if(mkdir(DirName, 0755) == -1)
              {
                  perror("mkdir   error");
                  return -1;
              }
            }
            DirName[i] = '/';
        }
    }
	return 0;
}

char *Trim(char *a_pcString, char a_cCharToTrim)
{
    if (a_pcString == NULL)
    {
        return NULL;
    }

    size_t iStrLen = 0;
    unsigned int iBegin = 0;
    unsigned int iEnd = 0;

    iStrLen = strlen(a_pcString);

    /* remove spaces at the begin*/
    for (iBegin=0; iBegin<iStrLen; iBegin++)
    {
        if(a_pcString[iBegin] != a_cCharToTrim)
        {
            break;
        }
    }

    /* remove spaces at the end */
    for (iEnd=0; iEnd<iStrLen;iEnd++)
    {
        if(a_pcString[iStrLen-1-iEnd] != a_cCharToTrim)
        {
            break;
        }
    }

    /* get the new length of string */
    iEnd = (unsigned int)iStrLen - iEnd;
    a_pcString[iEnd] = '\0';

    /* memory operate */
    if (iBegin != 0 && iEnd-iBegin+1 > 0)
    {
        memmove(a_pcString, a_pcString+iBegin, iEnd-iBegin+1);
    }

    return a_pcString;
}

char *Trim2(char *a_pcString, char a_cCharToTrim)
{
    if (a_pcString == NULL)
    {
        return NULL;
    }
    size_t iStrLen = 0;
    unsigned int iBegin = 0;
    unsigned int iEnd = 0;

    iStrLen = strlen(a_pcString);

    // process special situation
    if (iStrLen == 2 && a_pcString[0] == a_cCharToTrim && a_pcString[1] == a_cCharToTrim)
    {
        a_pcString[0] = 0;
        return a_pcString;
    }

    iBegin = 0;
    iEnd = 0;

    for (unsigned int i=0; i<iStrLen; i++)
    {
        if (a_pcString[i] == a_cCharToTrim && a_pcString[iStrLen-i-1] == a_cCharToTrim)
        {
            iBegin = i+1;
            iEnd = (unsigned int)iStrLen-i-1;
        }else
        {
            break;
        }
    }

    if (iBegin != 0 && iEnd != 0)
    {
        // remove last segment
        a_pcString[iEnd] = 0;

        // remove leading segment
        if (iBegin != 0 && iEnd-iBegin+1 > 0)
        {
            memmove(a_pcString, a_pcString+iBegin, iEnd-iBegin+1);
        }
    }

    return a_pcString;
}

int SplitString(const char * pszString, char cDelimiter, std::vector<string> &vecStrings)
{
    if (pszString == NULL)
    {
        return 0;
    }
    unsigned int iStrLen = 0;

    iStrLen = strlen(pszString);
    if (iStrLen <= 0)
    {
        return 0;
    }

    unsigned int i = 0;
    char* pcStr = NULL;
    char  cTempString[MAX_STRING_LENGTH] = {0,};
    bool  bFieldEnd = true;

    pcStr = (char *)pszString;
    for(i = 0; i < iStrLen; i++)
    {
        if (pszString[i] == '"')
        {   // enter field embodied by '"'.
            bFieldEnd = !bFieldEnd;
        }
        if (bFieldEnd && pszString[i] == cDelimiter)
        {
            // get the field's content
            memcpy(cTempString, pcStr, (pszString + i - pcStr));
            cTempString[pszString + i - pcStr] = '\0';

            // remove extra space
            Trim(cTempString, ' ');

            // remove extra double quotation marks
            Trim2(cTempString, '\"');

            // add to string's array
            vecStrings.push_back(string(cTempString));

            // move to next fields
            pcStr = (char *)(pszString + i + 1);
        }
    }

    if ((pszString[iStrLen-1] != cDelimiter) && (pcStr != pszString + iStrLen))
    {
        memcpy(cTempString, pcStr, (pszString + iStrLen - pcStr));
        cTempString[pszString + iStrLen - pcStr] = '\0';

        // remove extra space
        Trim(cTempString, ' ');

        Trim(cTempString, '\n');

        // remove extra double quotation marks
        Trim2(cTempString, '\"');

        // add to string's array
        vecStrings.push_back(string(cTempString));
    }

    return 1;
}

int GetVecFileList(const char * pszInPath, const char * pszFileSpec,
		vector<string>& vecpaths, vector<string>& vecfiles)
{
    if(pszInPath == NULL || strlen(pszInPath) <= 0)
    {
        return false;
    }

    char childpath[512];
    long lFileLen = strlen(pszInPath);
    memset(childpath, '\0', sizeof(childpath));
    memcpy(childpath, pszInPath, lFileLen);
    if (pszInPath[lFileLen - 1] != '/')
        strcat(childpath, "/");
    struct dirent* ent = NULL;
    DIR* pDir = NULL;
    pDir = opendir(pszInPath);
    while(NULL != (ent = readdir(pDir))) {
        if (ent->d_type & DT_DIR) {
            if (strcmp(ent->d_name, ".") == 0 ||
                    strcmp(ent->d_name, "..") == 0) continue;
            sprintf(childpath, "%s%s/", pszInPath, ent->d_name);
            GetVecFileList(childpath, pszFileSpec, vecpaths, vecfiles);
        }
        else {
            //int size = strlen(ent->d_name);
            //if (strcmp((ent->d_name + (size - 4)), pszFileSpec + 1) != 0) continue;
            vecpaths.push_back(childpath);
            vecfiles.push_back(ent->d_name);
        }
    }
    return true;
}

void Category2Star(const char* file_path, std::map<int, int>& map_category_star, std::set<int>& set_level) {
	FILE* fp = NULL;
	if ((fp = fopen(file_path, "rt")) == NULL)
		return;

	char cTempBuff[MAX_STRING_LENGTH];
	memset(cTempBuff, '\0', sizeof(cTempBuff));
	std::vector<string> vecFields;
	vecFields.clear();
	int category, star_level;
	try {
		while (!feof(fp)) {
			fgets(cTempBuff, MAX_STRING_LENGTH, fp);
			SplitString(cTempBuff, ',', vecFields);
			if (vecFields.size() != 3) {
				vecFields.clear();
				memset(cTempBuff, '\0', sizeof(cTempBuff));
				continue;
			}
			category = atoi(vecFields[0].c_str());
			star_level = atoi(vecFields[1].c_str());
			set_level.insert(star_level);
			map_category_star.insert(pair<int, int>(category, star_level));
			vecFields.clear();
			memset(cTempBuff, '\0', sizeof(cTempBuff));
		}
	}
	catch(...) {
		cout << "error file name:" << file_path << endl;
	}
	fclose(fp);
}

void FileOpen(const char* pFileName, const char* leavefilepath, map<int, int>& map_category_star, map<int, FILE*>& map_star_files) {
    FILE* fp = NULL;
    if ((fp = fopen(pFileName, "rt")) == NULL)
        return;

    FILE* foutput = NULL;
    if ((foutput = fopen(leavefilepath, "wt")) == NULL)
    	return;

    char cTempBuff[MAX_STRING_LENGTH];
    memset(cTempBuff, '\0', sizeof(cTempBuff));
    std::vector<string> vecFields;
    vecFields.clear();
    int poiCategoryId, starLevel, error_count;
    error_count = 0;
    map<int, int>::iterator iter;
    map<int, FILE*>::iterator iter_files;
    try {
		while (!feof(fp)) {
			fgets(cTempBuff, MAX_STRING_LENGTH, fp);
			SplitString(cTempBuff, '\t', vecFields);
			if (vecFields.size() != 29) {
				error_count++;
				vecFields.clear();
				memset(cTempBuff, '\0', sizeof(cTempBuff));
				continue;
			}

			poiCategoryId = atoi(vecFields[9].c_str());
			iter = map_category_star.find(poiCategoryId);
			if (iter != map_category_star.end()) {
				starLevel = iter->second;
				iter_files = map_star_files.find(starLevel);
				FILE* fstar = iter_files->second;
				fwrite(cTempBuff, strlen(cTempBuff), 1, fstar);
			} else {
				fwrite(cTempBuff, strlen(cTempBuff), 1, foutput);
			}
			vecFields.clear();
			memset(cTempBuff, '\0', sizeof(cTempBuff));
		}
    }
    catch(...) {
    	cout << "error file name:" << pFileName << endl;
    }
    fclose(fp);
    fclose(foutput);
}

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	//category to star level
	map<int, int> map_category_star;
	map<int, FILE*> map_star_files;
	map<int, FILE*>::iterator iter_files;
	std::set<int> star_level;
	std::set<int>::iterator iter_star;
	char file_path[512];
	string poi_source = "/home/ubuntu/Documents/poi_search/poi_data/source/";
	string star_file = "/home/ubuntu/Documents/poi_search/poi_data/category2star.txt";
	string star_output = "/home/ubuntu/Documents/poi_search/poi_data/star/";
	string poi_output = "/home/ubuntu/Documents/poi_search/poi_data/output/";

	CreateDir(star_output.c_str());
	vector<string> vecpath, vecfile;
	GetVecFileList(poi_source.c_str(), "", vecpath, vecfile);

	Category2Star(star_file.c_str(), map_category_star, star_level);
	//FILE* filelist[star_level.size()];
	//int i = 0;
	for (iter_star = star_level.begin(); iter_star != star_level.end(); ++iter_star) {
		int star_name = *iter_star;
		memset(file_path, '\0', sizeof(file_path));
		sprintf(file_path, "%s%d%s", star_output.c_str(), star_name, ".txt");
		//FILE* fp = filelist[i++];
		FILE* fp = NULL;
		if ((fp = fopen(file_path, "wt")) == NULL)
			continue;
		map_star_files.insert(std::pair<int, FILE*>(star_name, fp));
	}

	for(int i = 0; i < vecpath.size(); ++i) {
		string strfilepath = vecpath.at(i) + vecfile.at(i);
		std::size_t found = vecpath.at(i).rfind('/');
		string temp = vecpath.at(i).substr(0, found);
		std::size_t last = temp.rfind('/');
		temp = vecpath.at(i).substr(last+1, found);
		string leavefilepath = poi_output + temp;
		CreateDir(leavefilepath.c_str());
		leavefilepath = leavefilepath + vecfile.at(i) + ".txt";
		FileOpen(strfilepath.c_str(), leavefilepath.c_str(), map_category_star, map_star_files);
	}

	//release files
	for (iter_files = map_star_files.begin(); iter_files != map_star_files.end(); ++iter_files) {
		FILE* fp = iter_files->second;
		fclose(fp);
	}
	cout << "!!!success!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
