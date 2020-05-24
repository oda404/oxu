// Copyright (c) Olaru Alexandru <olarualexandru404@gmail.com>
// Licensed under the MIT license found in the LICENSE file in the root of this repository.

#include"mapManager.hpp"

/* because fuck that dogshit stoi function
    never works*/
uint parseIntFromStr(const std::string &str)
{
	uint integer = 0;

	for(unsigned int i = 0; i < str.size(); ++i)
		if((int)str[i] >= 48 && (int)str[i] <= 57)
			integer = integer * 10 + (int)str[i] - 48;

	return integer;
}

bool isBitOn(const uint8_t &number, const uint8_t &bit)
{
    if( (number & ( 1 << bit ) ) >> bit ) 
    {
        return true;
    }

    return false;
}

/* Returns the flags for the specified object */
uint8_t getObjCoreInfo(const std::string &line, uint infoArr[4])
{
    std::string buff;
    uint8_t     commaN = 0;
    uint8_t     flags = 0;

    for(const char &c: line)
    {
        /* If the current char is not a collon add it to a buffer */
        /* else if the char is a collon add/process the current buffer as obj info */
        if(c != ',')
        {
            buff += c;
        }
        else
        {
            if(commaN < 4)
            {
                if(commaN == 3)
                {
                    flags = parseIntFromStr(buff);
                    ++commaN;
                    buff.clear();
                }
                else
                {
                    infoArr[commaN++] = parseIntFromStr(buff);
                    buff.clear();
                }
            }
            else
            {
                break;
            }
        }      
    }
    return flags;
}

namespace oxu
{
    void MapManager::updateCombo(const uint8_t &flags)
    {
        isBitOn(flags, 2) ? combo = 1 : ++combo;
    }

    /* Populate the hitCircles vector in the MapInfo singleton */
    void MapManager::loadHitObjects(const int &songI, const int &mapI)
    {
        PlayField playField;
        MapInfo &mapInfoI = MapInfo::getInstance();

        std::ifstream infileMap(beatmaps[songI].second[mapI]);
        std::string line;

        bool shouldReadObjInf = false;

        while(std::getline(infileMap, line))
        {
            if(shouldReadObjInf)
            {
                /*The info array will be passed to the hit circle and contains:
                [0] = posX,
                [1] = posY
                [2] = hit time
                [3] = combo
                */
                unsigned int coreInfoArr[4];
                
                /* Parse the object line */
                uint8_t flags = getObjCoreInfo(line, coreInfoArr);

                updateCombo(flags);

                coreInfoArr[3] = combo;

                /* Add the hit circle */
                mapInfoI.hitCircles.emplace_back(coreInfoArr, playField);
            }
            else if(line == "[HitObjects]\r")
            {
                shouldReadObjInf = true;
            }
        }

        infileMap.close();
    }

    /* Go through every map in the songs folder
    *  and store it's path in a vector to be accessed
    *  with an index
    */
    void oxu::MapManager::enumBeatMaps()
    {
        beatmaps.clear();

        const char *mapFolderPath = "songs/";
        for(auto &p: fs::directory_iterator(mapFolderPath))
        {
            if(fs::is_directory(p))
            {
                std::pair<std::string, std::vector<std::string>> beatMap;
                beatMap.first = p.path();

                LOG_DEBUG("Loaded Song: {0}", p.path().filename().c_str());

                for(auto &file: fs::directory_iterator(p))
                {
                    if(file.path().extension() == ".osu")
                        beatMap.second.emplace_back(file.path());
                }
                beatmaps.emplace_back(beatMap);
            }
        }
    }

    /* ================== Difficulty ============================= */
    void oxu::MapManager::getMapDifficulty(std::ifstream &mapFile)
    {
        MapInfo &mapInfoI = MapInfo::getInstance();

        std::string line;
        bool go = false;

        while(std::getline(mapFile, line))
        {
            /* if it's still reading the difficulty section
            and encounters a blank line (new section) break */
            if(line == "\r" && go)
                break;

            if(go)
            {
                std::string keyValue  = line.substr(0, line.find_first_of(':'));
                float       diffValue = stof(line.substr(line.find_first_of(':') + 1));

                mapInfoI.addDifficultyAttr(keyValue, diffValue);

                /* transform the value into seconds */
                if(keyValue == "ApproachRate")
                {
                    if(diffValue <= 5)
                        mapInfoI.ARInSeconds = 1800 - diffValue * 120;
                    else
                        mapInfoI.ARInSeconds = 1200 - (diffValue - 5) * 150;

                    mapInfoI.ARInSeconds /= 1000;
                }
            }
            else if(line == "[Difficulty]\r")
            {
                go = true;
            }
        }
    }

    /* ====================== [General] ===================== */
    void oxu::MapManager::getMapGeneral(std::ifstream &mapFile)
    {
        MapInfo &mapInfoI = MapInfo::getInstance();

        std::string   line;
        bool          go = false;

        while(std::getline(mapFile, line))
        {
            if(line == "\r" && go)
                break;
            
            if(go)
            {
                std::string keyValue  = line.substr(0, line.find_first_of(':'));
                std::string diffValue = line.substr(line.find_first_of(':') + 1);

                /* Remove all spaces from string */
                diffValue.erase(std::remove_if(diffValue.begin(), diffValue.end(), isspace));
                diffValue.erase(diffValue.find_last_of('\r'));

                mapInfoI.addGeneralAttr(keyValue, diffValue);
            }
            else if(line == "[General]\r")
            {
                go = true;
            }
        }
    }

    /* ======================== [Metadata] ====================== */
    void oxu::MapManager::getMapMetadata(std::ifstream &mapFile)
    {
        MapInfo &mapInfoI = MapInfo::getInstance();

        std::string line;
        bool go = false;

        while(std::getline(mapFile, line))
        {
            if(line == "\r" && go)
                break;

            if(go)
            {
                std::string keyValue  = line.substr(0, line.find_first_of(':'));
                std::string diffValue = line.substr(line.find_first_of(':') + 1);

                /* Remove all spaces from string */
                diffValue.erase(std::remove_if(diffValue.begin(), diffValue.end(), isspace));

                mapInfoI.addMetadataAttr(keyValue, diffValue);
            }
            else if(line == "[Metadata]\r")
            {
                go = true;
            }
        }
    }

    /* Load the General, Metadata and Difficulty sections
    *  of the beatmap at the specified index
    */
    void oxu::MapManager::loadMapInfo(const int &songI, const int &mapI)
    {
        std::ifstream infileMap(beatmaps[songI].second[mapI]);

        getMapGeneral(infileMap);
        getMapMetadata(infileMap);
        getMapDifficulty(infileMap);
    }

    std::string oxu::MapManager::getSongPath(const int &index)
    {
        return beatmaps[index].first;
    }
}
