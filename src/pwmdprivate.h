/* vim:tw=78:ts=8:sw=4:set ft=cpp:  */
/*
    Copyright (C) 2022 Ben Kibbey <bjk@luxsci.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02110-1301  USA
*/
#ifndef PWMDPRIVATE_H
#define PWMDPRIVATE_H

#include "pwmd.h"
#include "profiles.h"

class UserAccount;

class PwmdPrivate : public Pwmd
{
    Q_OBJECT
    public:
	PwmdPrivate(const UserAccount &, QObject *);
	~PwmdPrivate();
        void getPassword();
        void savePassword();
        static bool checkRequirements();

    signals:
        void elementContentResult(gpg_error_t, QString);
        void saveElementContentResult(gpg_error_t, bool);

    private slots:
        void slotCommandResult(PwmdCommandQueueItem *, QString, gpg_error_t,
                               bool);

    private:
        UserAccount userAccount;

	QString buildElementPath(const QString &);
        void getElementContent();
        void saveElementContent();
        void getSaveCommon();
};

#endif
