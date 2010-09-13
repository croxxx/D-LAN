#ifndef GETHASHESRESULT_H
#define GETHASHESRESULT_H

#include <QObject>

#include <Protos/core_protocol.pb.h>

#include <IGetHashesResult.h>
#include <priv/Cache/Cache.h>
#include <priv/FileUpdater/FileUpdater.h>

namespace FM
{
   class Cache;
   class File;
   class FileUpdater;

   class GetHashesResult : public IGetHashesResult
   {
      Q_OBJECT
   public:
      GetHashesResult(const Protos::Common::FileEntry& fileEntry, Cache& cache, FileUpdater& fileUpdater);
      virtual ~GetHashesResult() {}
      Protos::Core::GetHashesResult start();

   private slots:
      void chunkHashKnown(QSharedPointer<Chunk> chunk);

   private:
      const Protos::Common::FileEntry& fileEntry;
      File* file; // TODO : if the file is deleted how can we know?
      Cache& cache;
      FileUpdater& fileUpdater;
   };
}

#endif
