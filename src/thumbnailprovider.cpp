#include "thumbnailprovider.h"

QImage ThumbnailProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Settings *settings = new Settings();
    if(!settings->getCacheThumbnails())
    {
        return QImage();
    }
    delete settings;

    QFileInfo fileInfo(id);

    QString idLC = id.toLower();
    // Check that the file is actually an image before creating a directory
    // and trying to create a thumbnail
    if (!idLC.endsWith(".png") && !idLC.endsWith(".jpg") && !idLC.endsWith(".jpeg")
            && !idLC.endsWith(".gif") && !idLC.endsWith(".svg"))
        return QImage();

    // Check that the directory for thumbnails exists
    QDir thumbDir(QString("%1/.thumbs").arg(fileInfo.absolutePath()));
    if (!thumbDir.exists())
    {
        bool succ = thumbDir.mkdir(QString("%1/.thumbs").arg(fileInfo.absolutePath()));
    }

    QString thumbnailPath = QString("%1/.thumbs/%2.cache").arg(fileInfo.absolutePath(), fileInfo.fileName());
    QImage thumbnailImage(thumbnailPath);

    if (thumbnailImage.isNull())
    {
        // Thumbnail not generated, generate it
        QImage originalImage(fileInfo.absoluteFilePath());

        int requestedHeight = requestedSize.height() == -1 ? 120 : requestedSize.height();
        int requestedWidth = requestedSize.width() == -1 ? 120 : requestedSize.width();

        originalImage = originalImage.scaled(requestedHeight, requestedWidth, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        // Save it
        bool success = originalImage.save(thumbnailPath, "PNG");

        return originalImage;
    }
    else
    {
        return thumbnailImage;
    }

    // Return null image if all else fails
    return QImage();
}
