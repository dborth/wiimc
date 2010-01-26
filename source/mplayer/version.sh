#!/bin/sh

test "$1" && extra="-$1"

# Extract revision number from file used by daily tarball snapshots
# or from the places different Subversion versions have it.
svn_revision=$(cat snapshot_version 2> /dev/null)
test $svn_revision || svn_revision=$(LC_ALL=C svn info 2> /dev/null | grep Revision | cut -d' ' -f2)
test $svn_revision || svn_revision=$(grep revision .svn/entries 2>/dev/null | cut -d '"' -f2)
test $svn_revision || svn_revision=$(sed -n -e '/^dir$/{n;p;q;}' .svn/entries 2>/dev/null)
test $svn_revision && svn_revision=SVN-r$svn_revision
test $svn_revision || svn_revision=UNKNOWN

# releases extract the version number from the VERSION file
version=$(cat VERSION 2> /dev/null)
test $version || version=$svn_revision

NEW_HEAD="#define VERSION \"${version}${extra}\"
#define BUILD_DATE \"$(date +%d/%m/%g)\""
OLD_HEAD=$(head -n 2 version.h 2> /dev/null)
TITLE='#define MP_TITLE "%s "VERSION" (C) 2000-2010 MPlayer Team\n"'

# Update version.h only on revision or date changes to avoid spurious rebuilds
if test "$NEW_HEAD" != "$OLD_HEAD"; then
    cat <<EOF > version.h
$NEW_HEAD
$TITLE
EOF
fi
