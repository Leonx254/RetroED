#ifndef STAGECONFIG_V5_H
#define STAGECONFIG_V5_H

namespace RSDKv5
{

class StageConfig
{
public:
    class WAVConfiguration
    {
    public:
        QString path           = "SFX.wav";
        byte maxConcurrentPlay = 1;

        WAVConfiguration() {}
        WAVConfiguration(QString name, byte maxPlays)
        {
            path              = name;
            maxConcurrentPlay = maxPlays;
        }
        WAVConfiguration(Reader &reader) { read(reader); }

        inline void read(Reader reader)
        {
            path              = reader.readString();
            maxConcurrentPlay = reader.read<byte>();
        }

        inline void write(Writer &writer)
        {
            writer.write(path);
            writer.write(maxConcurrentPlay);
        }
    };

    StageConfig() {}
    StageConfig(QString filename) { read(filename); }
    StageConfig(Reader &reader) { read(reader); }

    inline void read(QString filename)
    {
        Reader reader(filename);
        read(reader);
    }
    void read(Reader &reader);

    inline void write(QString filename)
    {
        if (filename == "")
            filename = m_filename;
        if (filename == "")
            return;
        Writer writer(filename);
        write(writer);
    }
    void write(Writer &writer);

    byte m_signature[4] = { 'C', 'F', 'G', 0 };

    bool loadGlobalObjects = true;
    QList<QString> objects;
    RSDKv5::Palette m_palettes[8];
    QList<WAVConfiguration> soundFX;

    QString m_filename = "";
};

} // namespace RSDKv5

#endif // STAGECONFIG_V5_H
