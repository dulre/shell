/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickItem>

#include <QtConfiguration/QConfiguration>

#include "containment.h"
#include "pluginloader.h"
#include "qmlobject.h"

namespace Hawaii {

namespace Shell {

/*
 * ContainmentPrivate
 */

class ContainmentPrivate
{
public:
    ContainmentPrivate();

    QConfiguration *configuration;
    Mantle *mantle;
    Types::ContainmentType type;
    Types::FormFactor formFactor;
    Types::Location location;
    Package package;
    QmlObject *qmlObject;
};

ContainmentPrivate::ContainmentPrivate()
    : mantle(nullptr)
    , type(Types::UnknownContainment)
    , formFactor(Types::Plane)
    , location(Types::Desktop)
{
}

/*
 * Containment
 */

Containment::Containment(Mantle *mantle, QObject *parent)
    : QObject(parent)
    , d_ptr(new ContainmentPrivate())
{
    Q_D(Containment);
    d->mantle = mantle;
    d->qmlObject = new QmlObject(this);
    d->qmlObject->setInitializationDelayed(true);

    // Save and load settings
    static int containmentId = 0;
    const QString section = QString("shell/%1/containments/containment%2")
            .arg(mantle->shell())
            .arg(QString::number(containmentId++));
    d->configuration = new QConfiguration(this, section, this);
}

Containment::~Containment()
{
    delete d_ptr;
}

Mantle *Containment::mantle() const
{
    Q_D(const Containment);
    return d->mantle;
}

Hawaii::Shell::Types::ContainmentType Containment::type() const
{
    Q_D(const Containment);
    return d->type;
}

Hawaii::Shell::Types::FormFactor Containment::formFactor() const
{
    Q_D(const Containment);
    return d->formFactor;
}

void Containment::setFormFactor(Types::FormFactor formFactor)
{
    Q_D(Containment);

    if (d->formFactor != formFactor) {
        d->formFactor = formFactor;
        Q_EMIT formFactorChanged(formFactor);
    }
}

Hawaii::Shell::Types::Location Containment::location() const
{
    Q_D(const Containment);
    return d->location;
}

void Containment::setLocation(Types::Location location)
{
    Q_D(Containment);

    if (d->location != location) {
        d->location = location;

        switch (d->location) {
        case Types::LeftEdge:
            setFormFactor(Types::Vertical);
            break;
        case Types::TopEdge:
            setFormFactor(Types::Horizontal);
            break;
        case Types::RightEdge:
            setFormFactor(Types::Vertical);
            break;
        case Types::BottomEdge:
            setFormFactor(Types::Horizontal);
            break;
        default:
            break;
        }

        Q_EMIT locationChanged(location);
    }
}

void Containment::setContextProperty(const QString &name, const QVariant &value)
{
    Q_D(Containment);
    d->qmlObject->engine()->rootContext()->setContextProperty(name, value);
}

Package Containment::package() const
{
    Q_D(const Containment);
    return d->package;
}

void Containment::setPackage(const Package &package)
{
    Q_D(Containment);

    QElapsedTimer timer;
    timer.start();

    qDebug() << "-> Loading containment" << package.metadata().internalName();

    // Read the containment type
    QString containmentType = package.metadata().property(
                QStringLiteral("ContainmentType")).toString();
    if (containmentType == QStringLiteral("desktop"))
        d->type = Types::DesktopContainment;
    else if (containmentType == QStringLiteral("panel"))
        d->type = Types::PanelContainment;
    else
        qFatal("Containment \"%s\" has an invalid type \"%s\", aborting...",
               qPrintable(package.metadata().internalName()),
               qPrintable(containmentType));

    // Load QML file
    d->qmlObject->setSource(QUrl::fromLocalFile(package.filePath("mainscript")));
    if (!d->qmlObject->engine() || !d->qmlObject->engine()->rootContext() ||
            !d->qmlObject->engine()->rootContext()->isValid() ||
            d->qmlObject->mainComponent()->isError()) {
        QStringList errorMsgs;
        for (QQmlError error: d->qmlObject->mainComponent()->errors())
            errorMsgs.append(error.toString());
        QString errorMsg = tr("Error loading QML file: %1").arg(errorMsgs.join('\n'));
        qWarning("  ** %s", qPrintable(errorMsg));

        // Load the element error component from mantle's package
        d->qmlObject->setSource(QUrl::fromLocalFile(mantle()->package().filePath("elementerror")));
        d->qmlObject->completeInitialization();

        // If even the element error component cannot be loaded this is a lost cause
        if (d->qmlObject->mainComponent()->isError())
            return;

        // Element error component was loaded, set error message
        d->qmlObject->rootObject()->setProperty("errorMessage", errorMsg);
    }

    // Load is complete
    d->qmlObject->completeInitialization();

    qDebug() << "  Containment created in" << timer.elapsed() << "ms";

    // Keep track of the root item
    QQuickItem *rootItem = qobject_cast<QQuickItem *>(d->qmlObject->rootObject());
    if (rootItem)
        setProperty("item", QVariant::fromValue(rootItem));

    // Assign the package and notify observers
    d->package = package;
    Q_EMIT packageChanged(package);
}

} // namespace Shell

} // namespace Hawaii

#include "moc_containment.cpp"
