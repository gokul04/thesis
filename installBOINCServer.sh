#!/bin/bash
#
# Script to install the BOINC Server in a Debian machine
#

BOINC_PROJ_NAME=""
BOINC_PROJ_LONG_NAME=""

#DEFAULT:
ADMIN_USER="desktopgrid"
ADMIN_USER_PWD="boinc"
BOINC_USER="boinc-"$BOINC_PROJ_NAME

## First adding the PPA to the sources.list file
SZDG_PPA="\ndeb http://www.desktopgrid.hu/debian/ squeeze szdg\n"
echo $SZDG_PPA >> /etc/apt/sources.list

wget http://www.desktopgrid.hu/szdg-key.gpg
apt-key add szdg.key.gpg
apt-get update

## Installing the Server packages
apt-get install boinc-server boinc-skin-standard vim

## setting MySQL root access 
MYSQL_CONF_FILE=/root/.my.cnf
echo "[mysql]" > $MYSQL_CONF_FILE
echo "user = root" >> $MYSQL_CONF_FILE
echo "password = bombay" >> $MYSQL_CONF_FILE
echo "" >> $MYSQL_CONF_FILE
echo "[mysqladmin]" >> $MYSQL_CONF_FILE
echo "user = root" >> $MYSQL_CONF_FILE
echo "password = bombay" >> $MYSQL_CONF_FILE

## Create BOINC project
boinc_create_project --name=$BOINC_PROJ_NAME --long-name=$BONIC_PROJ_LONG_NAME

## Add an admin user
adduser --disabled-password --gecos ""  $ADMIN_USER
echo $ADMIN_USER:$ADMIN_USER_PWD |chpasswd
boinc_admin --name=$BOINC_PROJ_NAME --add $ADMIN_USER

## Install packages for 3G Bridge
apt-get install 3g-bridge 3g-bridge-wsclient
COPY_SCHEMA="mysql boinc_$BOINC_PROJ_NAME < /usr/share/3g-bridge/schema.sql"
su - $BOINC_USER -c $COPY_SCHEMA
ADD_MASTER="boinc_appmgr --add --master /usr/share/3g-bridge/master-ws.xml"
su - $BOINC_USER -c $ADD_MASTER

