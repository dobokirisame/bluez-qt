/*
 * BluezQt - Asynchronous Bluez wrapper library
 *
 * Copyright (C) 2015 David Rosca <nowrep@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "declarativemediaplayer.h"

DeclarativeMediaPlayer::DeclarativeMediaPlayer(BluezQt::MediaPlayerPtr mediaPlayer, QObject *parent)
    : QObject(parent)
    , m_mediaPlayer(mediaPlayer)
{
    connect(m_mediaPlayer.data(), &BluezQt::MediaPlayer::nameChanged, this, &DeclarativeMediaPlayer::nameChanged);
    connect(m_mediaPlayer.data(), &BluezQt::MediaPlayer::equalizerChanged, this, &DeclarativeMediaPlayer::equalizerChanged);
    connect(m_mediaPlayer.data(), &BluezQt::MediaPlayer::repeatChanged, this, &DeclarativeMediaPlayer::repeatChanged);
    connect(m_mediaPlayer.data(), &BluezQt::MediaPlayer::shuffleChanged, this, &DeclarativeMediaPlayer::shuffleChanged);
    connect(m_mediaPlayer.data(), &BluezQt::MediaPlayer::statusChanged, this, &DeclarativeMediaPlayer::statusChanged);
    connect(m_mediaPlayer.data(), &BluezQt::MediaPlayer::positionChanged, this, &DeclarativeMediaPlayer::positionChanged);

    connect(m_mediaPlayer.data(), &BluezQt::MediaPlayer::trackChanged, this, [this]() {
        updateTrack();
        Q_EMIT trackChanged(m_track);
    });

    updateTrack();
}

QString DeclarativeMediaPlayer::name() const
{
    return m_mediaPlayer->name();
}

BluezQt::MediaPlayer::Equalizer DeclarativeMediaPlayer::equalizer() const
{
    return m_mediaPlayer->equalizer();
}

void DeclarativeMediaPlayer::setEqualizer(BluezQt::MediaPlayer::Equalizer equalizer)
{
    m_mediaPlayer->setEqualizer(equalizer);
}

BluezQt::MediaPlayer::Repeat DeclarativeMediaPlayer::repeat() const
{
    return m_mediaPlayer->repeat();
}

void DeclarativeMediaPlayer::setRepeat(BluezQt::MediaPlayer::Repeat repeat)
{
    m_mediaPlayer->setRepeat(repeat);
}

BluezQt::MediaPlayer::Shuffle DeclarativeMediaPlayer::shuffle() const
{
    return m_mediaPlayer->shuffle();
}

void DeclarativeMediaPlayer::setShuffle(BluezQt::MediaPlayer::Shuffle shuffle)
{
    m_mediaPlayer->setShuffle(shuffle);
}

BluezQt::MediaPlayer::Status DeclarativeMediaPlayer::status() const
{
    return m_mediaPlayer->status();
}

QJsonObject DeclarativeMediaPlayer::track() const
{
    return m_track;
}

quint32 DeclarativeMediaPlayer::position() const
{
    return m_mediaPlayer->position();
}

BluezQt::PendingCall *DeclarativeMediaPlayer::play()
{
    return m_mediaPlayer->play();
}

BluezQt::PendingCall *DeclarativeMediaPlayer::pause()
{
    return m_mediaPlayer->pause();
}

BluezQt::PendingCall *DeclarativeMediaPlayer::stop()
{
    return m_mediaPlayer->stop();
}

BluezQt::PendingCall *DeclarativeMediaPlayer::next()
{
    return m_mediaPlayer->next();
}

BluezQt::PendingCall *DeclarativeMediaPlayer::previous()
{
    return m_mediaPlayer->previous();
}

BluezQt::PendingCall *DeclarativeMediaPlayer::fastForward()
{
    return m_mediaPlayer->fastForward();
}

BluezQt::PendingCall *DeclarativeMediaPlayer::rewind()
{
    return m_mediaPlayer->rewind();
}

void DeclarativeMediaPlayer::updateTrack()
{
    BluezQt::MediaPlayerTrackPtr t = m_mediaPlayer->track();

    m_track[QStringLiteral("title")] = t ? t->title() : QString();
    m_track[QStringLiteral("artist")] = t ? t->artist() : QString();
    m_track[QStringLiteral("album")] = t ? t->album() : QString();
    m_track[QStringLiteral("genre")] = t ? t->genre() : QString();
    m_track[QStringLiteral("numberOfTracks")] = qint64(t ? t->numberOfTracks() : 0);
    m_track[QStringLiteral("trackNumber")] = qint64(t ? t->trackNumber() : 0);
    m_track[QStringLiteral("duration")] = qint64(t ? t->duration() : 0);
}