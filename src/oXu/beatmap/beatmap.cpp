#include "beatmap.hpp"

#include<fstream>
#include<mutex>

#include<oXu/beatmap/beatmapParser.hpp>
#include<oXu/beatmap/sections/config.hpp>
#include<oXu/beatmap/components/playField.hpp>
#include<oXu/core/logger.hpp>

namespace oxu
{
    static std::mutex mtx;

    static void setSection(const std::string &line, uint8_t &section)
    {
        std::map<std::string, std::uint8_t>::const_iterator pair = sectionsMap.find(line);

        if(pair->first != "")
        {
            section = pair->second;
        }
    }

    Beatmap::Beatmap(const std::string &path_p):
    m_path(path_p)
    {

    }

    void Beatmap::start()
    {
        m_objI = 0;
        m_hitObjectsPool.clear();
        m_timer.start();
    }

    void Beatmap::updateObjects(const double &delta)
    {
        static uint32_t ellapsedMs;
        ellapsedMs = m_timer.getEllapsedMs();

        mtx.lock();
        while(m_hitObjects[m_objI]->shouldBeAddedToPool(ellapsedMs))
        {
            m_hitObjectsPool.push_back(m_hitObjects[m_objI].get());
            ++m_objI;
        }
        while(m_hitObjectsPool.size() > 0 && m_hitObjectsPool[0]->shouldBeRemovedFromPool())
        {
            m_hitObjectsPool.erase(m_hitObjectsPool.begin());
        }
        for(HitObject *ho: m_hitObjectsPool)
        {
            ho->update(delta, difficulty);
        }
        mtx.unlock();
    }

    void Beatmap::renderObjects(const Skin &skin)
    {
        mtx.lock();
        std::uint32_t i = m_hitObjectsPool.size(); 
        for(; i > 0; --i)
        {
            m_hitObjectsPool[i - 1]->render(skin);
        }
        mtx.unlock();
    }

    void Beatmap::loadGenericInfo()
    {
        std::ifstream beatmapFile(m_path);

        if(beatmapFile.is_open())
        {
            std::string line;
            uint8_t section = SECTIONS_COUNT;

            while(std::getline(beatmapFile, line))
            {
                if(line[0] == '[')
                {
                    if(section == Sections::DIFFICULTY)
                    {
                        break;
                    }

                    setSection(line, section);
                }
                else if(line[0] != '\r')
                {
                    switch(section)
                    {
                    case Sections::GENERAL:
                        parseAndSetGeneral(line, general);
                        break;

                    case Sections::METADATA:
                        parseAndSetMetadata(line, metadata);
                        break;

                    case Sections::DIFFICULTY:
                        parseAndSetDifficulty(line, difficulty);
                        break;
                    }
                }
            }

            beatmapFile.close();
        }
        else
        {
            OXU_LOG_WARN("Could not open file {0}", m_path);
        }
    }

    void Beatmap::loadGameInfo()
    {
        std::ifstream beatmapFile(m_path);

        if(beatmapFile.is_open())
        {
            m_hitObjects.clear();
            /* hardcoded value for now */
            m_hitObjects.reserve(4000);

            std::string line;
            uint8_t section = SECTIONS_COUNT;
            PlayField playField;

            while(std::getline(beatmapFile, line))
            {
                if(line[0] == '[')
                {
                    setSection(line, section);
                }
                else if(line[0] != '\r')
                {
                    switch(section)
                    {
                        case Sections::TIMING:

                            break;

                        case Sections::OBJECTS:
                            parseAndAddHitObject(line, m_hitObjects, playField, difficulty);
                            break;
                    }
                }
            }

            beatmapFile.close();
        }
        else
        {
            OXU_LOG_WARN("Could not open file {0}", m_path);
        }
    }

    const std::string &Beatmap::getPath()
    {
        return m_path;
    }
}
