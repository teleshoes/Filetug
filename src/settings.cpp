#include "settings.h"

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    qDebug() << "Reading config";

    loadBookmarks();

    settings = new QSettings("Matoking", "Filetug");

    // Get stored values
    if (settings->contains("dirPath"))
        m_dirPath = settings->value("dirPath").toString();
    else
        m_dirPath = "/home/nemo"; // Default path

    if (settings->contains("defaultViewMode"))
        m_defaultViewMode = settings->value("defaultViewMode").toString();
    else
        m_defaultViewMode = "grid";

    if (settings->contains("showShortcutsAtStartup"))
        m_showShortcutsAtStartup = settings->value("showShortcutsAtStartup").toBool();
    else
        m_showShortcutsAtStartup = true;

    if (settings->contains("showHiddenFiles"))
        m_showHiddenFiles = settings->value("showHiddenFiles").toBool();
    else
        m_showHiddenFiles = false;

    if (settings->contains("showDirHeader"))
        m_showDirHeader = settings->value("showDirHeader").toBool();
    else
        m_showDirHeader = true;

    if (settings->contains("galleryMode"))
        m_galleryMode = settings->value("galleryMode").toBool();
    else
        m_galleryMode = true;

    if (settings->contains("displayThumbnails"))
        m_displayThumbnails = settings->value("displayThumbnails").toBool();
    else
        m_displayThumbnails = true;

    if (settings->contains("cacheThumbnails"))
        m_cacheThumbnails = settings->value("cacheThumbnails").toBool();
    else
        m_cacheThumbnails = false;

    if (settings->contains("sortBy"))
        m_sortBy = settings->value("sortBy").toString();
    else
        m_sortBy = "name";

    if (settings->contains("sortOrder"))
        m_sortOrder = settings->value("sortOrder").toString();
    else
        m_sortOrder = "asc";

    if (settings->contains("dirOrder"))
        m_dirOrder = settings->value("dirOrder").toString();
    else
        m_dirOrder = "first";

    if (settings->contains("fileOverlayPeriod"))
        m_fileOverlayPeriod = settings->value("fileOverlayPeriod").toFloat();
    else
        m_fileOverlayPeriod = 2;

    if (settings->contains("browseAllFileTypes"))
        m_browseAllFileTypes = settings->value("browseAllFileTypes").toBool();
    else
        m_browseAllFileTypes = false;

    if (settings->contains("showBlackBackground"))
        m_showBlackBackground = settings->value("showBlackBackground").toBool();
    else
        m_showBlackBackground = true;
}

void Settings::loadBookmarks()
{
    // Try creating the directory if it doesn't exist
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if (!dir.exists())
        dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    // Load the bookmark list
    QFile bookmarkFile(QString("%1/bookmarks.json").arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation)));

    bookmarkFile.open(QIODevice::ReadOnly);

    if (bookmarkFile.exists())
    {
        QJsonArray bookmarkArray = QJsonDocument::fromJson(bookmarkFile.readAll()).array();

        m_bookmarkMap.clear();

        for (int i=0; i < bookmarkArray.size(); i++)
        {
            QJsonObject bookmarkEntry = bookmarkArray.at(i).toObject();

            m_bookmarkMap.insert(bookmarkEntry.value("path").toString(), bookmarkEntry.value("title").toString());
        }
    }

    bookmarkFile.close();
}

void Settings::saveBookmarks()
{
    QFile bookmarkFile(QString("%1/bookmarks.json").arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation)));

    bookmarkFile.open(QIODevice::WriteOnly);

    QJsonArray jsonArray;

    QMapIterator<QString, QVariant> i(m_bookmarkMap);
    while (i.hasNext())
    {
        i.next();

        QString path = i.key();
        QString title = i.value().toString();

        QJsonObject entryObject;
        entryObject.insert("path", QJsonValue::fromVariant(QVariant::fromValue(path)));
        entryObject.insert("title", QJsonValue::fromVariant(QVariant::fromValue(title)));

        jsonArray.append(QJsonValue(entryObject));
    }

    // Save bookmark list as JSON-encoded string
    QJsonDocument jsonDocument(jsonArray);

    QByteArray jsonString = jsonDocument.toJson();

    bookmarkFile.resize(0);
    bookmarkFile.write(jsonString);

    bookmarkFile.flush();
    bookmarkFile.close();
}

QVariant Settings::getBookmarks()
{
    return QVariant::fromValue(m_bookmarkMap);
}

/*
 *  Adds a bookmark path with a title
 */
void Settings::addBookmarkPath(const QString &dirPath, const QString &title)
{
    // Delete the existing bookmark if it exists
    m_bookmarkMap.remove(dirPath);

    m_bookmarkMap.insert(dirPath, title);

    saveBookmarks();
}

void Settings::removeBookmarkPath(const QString &dirPath)
{
    if (m_bookmarkMap.contains(dirPath))
        m_bookmarkMap.remove(dirPath);

    saveBookmarks();
}

bool Settings::isPathInBookmarks(const QString &dirPath)
{
    return m_bookmarkMap.contains(dirPath);
}

/*
 *  dirPath - currently displayed directory
 */
void Settings::setDirPath(const QString &dirPath)
{
    if (m_dirPath != dirPath)
    {
        m_dirPath = dirPath;
        settings->setValue("dirPath", dirPath);
        settings->sync();
        emit dirPathChanged(dirPath);
    }
}

QString Settings::getDirPath() const
{
    return m_dirPath;
}

/*
 *  Go up in the directory hierarchy
 */
void Settings::cdUp()
{
    QDir dir(getDirPath());

    bool success = dir.cdUp();

    if (success)
        setDirPath(dir.absolutePath());
}

/*
 *  defaultViewMode - default view mode when viewing directories
 */
void Settings::setDefaultViewMode(const QString &defaultViewMode)
{
    if (m_defaultViewMode != defaultViewMode && (defaultViewMode == "grid" || defaultViewMode == "list"))
    {
        m_defaultViewMode = defaultViewMode;
        settings->setValue("defaultViewMode", defaultViewMode);
        settings->sync();
        emit defaultViewModeChanged(defaultViewMode);
        emit directoryViewSettingsChanged();
    }
}

QString Settings::getDefaultViewMode() const
{
    return m_defaultViewMode;
}

/*
 *  showShortcutsAtStartup - show the shortcuts view at startup instead of the last location
 */
void Settings::setShowShortcutsAtStartup(const bool &showShortcutsAtStartup)
{
    if (m_showShortcutsAtStartup != showShortcutsAtStartup)
    {
        m_showShortcutsAtStartup = showShortcutsAtStartup;
        settings->setValue("showShortcutsAtStartup", QVariant(showShortcutsAtStartup));
        settings->sync();
        emit showShortcutsAtStartupChanged(showShortcutsAtStartup);
        emit directoryViewSettingsChanged();
    }
}

bool Settings::getShowShortcutsAtStartup() const
{
    return m_showShortcutsAtStartup;
}

/*
 *  showHiddenFiles - show hidden files and folders
 */
void Settings::setShowHiddenFiles(const bool &showHiddenFiles)
{
    if (m_showHiddenFiles != showHiddenFiles)
    {
        m_showHiddenFiles = showHiddenFiles;
        settings->setValue("showHiddenFiles", QVariant(showHiddenFiles));
        settings->sync();
        emit showHiddenFilesChanged(showHiddenFiles);
        emit directoryViewSettingsChanged();
    }
}

bool Settings::getShowHiddenFiles() const
{
    return m_showHiddenFiles;
}

/*
 *  showDirHeader - show the page header in directory view
 */
void Settings::setShowDirHeader(const bool &showDirHeader)
{
    if (m_showDirHeader != showDirHeader)
    {
        m_showDirHeader = showDirHeader;
        settings->setValue("showDirHeader", QVariant(showDirHeader));
        settings->sync();
        emit showDirHeaderChanged(showDirHeader);
        emit directoryViewSettingsChanged();
    }
}

bool Settings::getShowDirHeader() const
{
    return m_showDirHeader;
}

/*
 *  galleryMode - use grid view automatically when viewing a directory containing image and/or video files
 */
void Settings::setGalleryMode(const bool &galleryMode)
{
    if (m_galleryMode != galleryMode)
    {
        m_galleryMode = galleryMode;
        settings->setValue("galleryMode", QVariant(galleryMode));
        settings->sync();
        emit galleryModeChanged(galleryMode);
        emit directoryViewSettingsChanged();
    }
}

bool Settings::getGalleryMode() const
{
    return m_galleryMode;
}

/*
 *  displayThumbnails - display thumbnails for image and video files
 */
void Settings::setDisplayThumbnails(const bool &displayThumbnails)
{
    if (m_displayThumbnails != displayThumbnails)
    {
        m_displayThumbnails = displayThumbnails;
        settings->setValue("displayThumbnails", QVariant(displayThumbnails));
        settings->sync();
        emit displayThumbnailsChanged(displayThumbnails);
    }
}

bool Settings::getDisplayThumbnails() const
{
    return m_displayThumbnails;
}

/*
 *  cacheThumbnails - save generated thumbnails for faster loading
 */
void Settings::setCacheThumbnails(const bool &cacheThumbnails)
{
    if (m_cacheThumbnails != cacheThumbnails)
    {
        m_cacheThumbnails = cacheThumbnails;
        settings->setValue("cacheThumbnails", QVariant(cacheThumbnails));
        settings->sync();
        emit cacheThumbnailsChanged(cacheThumbnails);
    }
}

bool Settings::getCacheThumbnails() const
{
    return m_cacheThumbnails;
}

/*
 *  sortBy - sort files by name, type, size or time
 */
void Settings::setSortBy(const QString &sortBy)
{
    if (m_sortBy != sortBy)
    {
        m_sortBy = sortBy;
        settings->setValue("sortBy", QVariant(sortBy));
        settings->sync();
        emit sortByChanged(sortBy);
        emit directoryViewSettingsChanged();
    }
}

QString Settings::getSortBy() const
{
    return m_sortBy;
}

/*
 *  sortOrder - sort files either in an ascending or descending order
 */
void Settings::setSortOrder(const QString &sortOrder)
{
    if (m_sortOrder != sortOrder)
    {
        m_sortOrder = sortOrder;
        settings->setValue("sortOrder", QVariant(sortOrder));
        settings->sync();
        emit sortOrderChanged(sortOrder);
        emit directoryViewSettingsChanged();
    }
}

QString Settings::getSortOrder() const
{
    return m_sortOrder;
}

/*
 *  dirOrder - show directories either first or last or don't order them at all
 */
void Settings::setDirOrder(const QString &dirOrder)
{
    if (m_dirOrder != dirOrder)
    {
        m_dirOrder = dirOrder;
        settings->setValue("dirOrder", QVariant(dirOrder));
        settings->sync();
        emit dirOrderChanged(dirOrder);
        emit directoryViewSettingsChanged();
    }
}

QString Settings::getDirOrder() const
{
    return m_dirOrder;
}

/*
 *  fileOverlayPeriod - how long the file overlay will stay visible after it has been invoked
 */
void Settings::setFileOverlayPeriod(const float &fileOverlayPeriod)
{
    if (m_fileOverlayPeriod != fileOverlayPeriod)
    {
        m_fileOverlayPeriod = fileOverlayPeriod;
        settings->setValue("fileOverlayPeriod", QVariant(fileOverlayPeriod));
        settings->sync();
        emit fileOverlayPeriodChanged(fileOverlayPeriod);
        emit fileDisplaySettingsChanged();
    }
}

float Settings::getFileOverlayPeriod() const
{
    return m_fileOverlayPeriod;
}

/*
 *  browseAllFileTypes - whether the file browser goes through all types of displayable
 *  files as opposed to files of the same type
 */
void Settings::setBrowseAllFileTypes(const bool &browseAllFileTypes)
{
    if (m_browseAllFileTypes != browseAllFileTypes)
    {
        m_browseAllFileTypes = browseAllFileTypes;
        settings->setValue("browseAllFileTypes", QVariant(browseAllFileTypes));
        settings->sync();
        emit browseAllFileTypesChanged(browseAllFileTypes);
        emit fileDisplaySettingsChanged();
    }
}

bool Settings::getBrowseAllFileTypes() const
{
    return m_browseAllFileTypes;
}

/*
 *  showBlackBackground - show a black background in image and video views
 */
void Settings::setShowBlackBackground(const bool &showBlackBackground)
{
    if (m_showBlackBackground != showBlackBackground)
    {
        m_showBlackBackground = showBlackBackground;
        settings->setValue("showBlackBackground", QVariant(showBlackBackground));
        settings->sync();
        emit showBlackBackgroundChanged(showBlackBackground);
        emit fileDisplaySettingsChanged();
    }
}

bool Settings::getShowBlackBackground() const
{
    return m_showBlackBackground;
}
