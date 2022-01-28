/*
 *  pwmdprivate.cpp - Subclasses Pwmd from the qpwmc module.
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
#include "pwmdprivate.h"
#include "psioptions.h"

#define PwmdCmdIdGetContent	0
#define PwmdCmdIdStoreContent	1
#define PwmdCmdIdPsiMax		2

static bool fetchingContent;

PwmdPrivate::PwmdPrivate(const UserAccount &acc, QObject *p) :
  Pwmd(0, "psi", 0, p)
{
    qRegisterMetaType <Pwmd::ConnectionState>("Pwmd::ConnectionState");
    qRegisterMetaType <gpg_error_t>("gpg_error_t");
    userAccount = acc;
    QObject::connect(this, SIGNAL(commandResult(PwmdCommandQueueItem *, QString, gpg_error_t, bool)), this, SLOT(slotCommandResult(PwmdCommandQueueItem *, QString, gpg_error_t, bool)));
}

PwmdPrivate::~PwmdPrivate()
{
}

void PwmdPrivate::getSaveCommon()
{
    PwmdRemoteHost hostData;

    setFilename(PsiOptions::instance()->getOption("options.libpwmd.dataFile").toString().trimmed());
    setSocket(PsiOptions::instance()->getOption("options.libpwmd.socket").toString().trimmed());
    pwmd_setopt(handle(), PWMD_OPTION_SOCKET_TIMEOUT, 120);

    if (PwmdRemoteHost::fillRemoteHost(socket(), hostData)) {
        setSocket(PwmdRemoteHost::socketUrl(hostData));
        setConnectParameters(hostData.socketArgs());
        pwmd_setopt(handle(), PWMD_OPTION_SSH_AGENT, hostData.sshAgent());
        pwmd_setopt(handle(), PWMD_OPTION_SSH_NEEDS_PASSPHRASE,
                    hostData.sshNeedsPassphrase());
        pwmd_setopt(handle(), PWMD_OPTION_SOCKET_TIMEOUT,
                    state() == Pwmd::Init ? hostData.connectTimeout()
                    : hostData.socketTimeout());
        pwmd_setopt(handle(), PWMD_OPTION_TLS_VERIFY, hostData.tlsVerify());

        if (!hostData.tlsPriority().isEmpty()) {
            pwmd_setopt (handle(), PWMD_OPTION_TLS_PRIORITY,
                         hostData.tlsPriority().toUtf8().data());
        }
    }

    connect();
}

void PwmdPrivate::getPassword()
{
    fetchingContent = true;
    getSaveCommon();
}

void PwmdPrivate::slotCommandResult(PwmdCommandQueueItem *item, QString result,
                                    gpg_error_t rc, bool queued)
{
    if (!item) {
        showError(rc);
        return;
    }

    if (item->id() >= PwmdCmdIdPsiMax) {
        item->setSeen ();
        return;
    }

    switch (item->id()) {
    case PwmdCmdIdInternalStatusError:
        reset();
        break;
    case PwmdCmdIdInternalConnect:
        if (!rc)
            open();
        else
            reset();
        break;
    case PwmdCmdIdInternalSave:
        if (!rc)
            emit saveElementContentResult(rc, true);
        break;
    case PwmdCmdIdInternalOpen:
        if (!rc && fetchingContent)
            getElementContent();
        else if (!rc)
            saveElementContent();
        break;
    case PwmdCmdIdGetContent:
        if (!rc)
            emit elementContentResult(rc, result);
        break;
    case PwmdCmdIdStoreContent:
        if (!rc)
            emit saveElementContentResult(rc, false);
        break;
    default:
        break;
    }

    if (rc) {
        if (fetchingContent)
            emit elementContentResult(rc, QString());
        else
            emit saveElementContentResult(rc, false);

        showError(rc);
    }

    item->setSeen();
}

/* The password element should be stored as:
 *     <jid>
 *         <password>some password</password>
 *     </jid>
 */
QString PwmdPrivate::buildElementPath(const QString &element)
{
    QString path = PsiOptions::instance()->getOption("options.libpwmd.rootElement").toString().trimmed();

    if (!path.isEmpty()) {
        for (int i = 0; i < path.length(); i++) {
            QChar c = path.at(i);

            if (c == '^')
                path.replace(i, 1, "\t");
        }
        path.append("\t");
    }

    path.append(userAccount.jid);

    if (!element.isEmpty())
        path += "\t" + element;

    return path;
}

void PwmdPrivate::getElementContent()
{
    fetchingContent = false;
    PwmdInquireData *inq = new PwmdInquireData(buildElementPath("password"));
    command(new PwmdCommandQueueItem(PwmdCmdIdGetContent, "GET",
                                     Pwmd::inquireCallback, inq));
}

void PwmdPrivate::saveElementContent() {
    PwmdInquireData *inq = new PwmdInquireData(buildElementPath("password")
                                               + "\t" + userAccount.pass);
    command(new PwmdCommandQueueItem(PwmdCmdIdStoreContent, "STORE",
                                     Pwmd::inquireCallback, inq));
}

void PwmdPrivate::savePassword()
{
    fetchingContent = false;
    getSaveCommon();
}

bool PwmdPrivate::checkRequirements() {
    QString s = PsiOptions::instance()->getOption("options.libpwmd.dataFile").toString().trimmed();

    return !s.isEmpty();
}
