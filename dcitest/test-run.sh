#!/bin/sh
# Run ocitest for OCI test of kingbase.
# For configuration of test, such as port, host,
# see test-config in tests_home.
#
# Portions Copyright (c) 2003-2012, BaseSoft Inc.
#                                  added by lluo.
tests_home="`pwd`/../.."
proj_home="$tests_home/../.."

. "$tests_home/test-config"
. "$tests_home/test-run.global"

kingbase_log="`pwd`/kingbase.log"
initdb_log="`pwd`/initdb.log"
OCITest_log="`pwd`/OCITest.log"
test_what="OCITest"
service_name="TEST_OCI"
OCITest_opt=" -n $service_name -U $test_user -W $test_pwd "
oci_install_path="$proj_home/src/oci/.libs"

echo_test_head $test_what

gather_log()
{
	rm -rf "$tests_home/log/$test_what"/*
	mkdir -p "$tests_home/log/$test_what"
	mv -f *.log "$tests_home/log/$test_what/"
	mv -f result.txt "$tests_home/log/$test_what/"
	cp -f expected.txt "$tests_home/log/$test_what/"
}

config_odbc "`pwd`/../../../odbc/.libs/"
printf "Initializing enviroment ...... "

mv -f "$kingbase_home/config/sys_dblink.conf" \
      "$kingbase_home/config/sys_dblink.conf.bak"
mv -f "$kingbase_home/config/sys_service.conf" \
      "$kingbase_home/config/sys_service.conf.bak"

echo "
[$service_name]
host=$test_host
port=$test_port
dbname=$test_db
UsePackage=1
UseDciDat=0
" >"$kingbase_home/config/sys_service.conf"

echo "
[kdb]
DriverType=ODBC
DriverName=\"KingbaseES 7 ODBC Driver\"
Host=$test_host
Port=$test_port
dbname=$test_db
" > "$kingbase_home/config/sys_dblink.conf"
printf "done\n"
export KINGBASE_CONFDIR="$kingbase_home/config"

if test x"$use_initkdb" = x"yes"; then
    init_kingbase "$initdb_log";
    export kingbase_opt="$kingbase_opt -c compatible_level=mixed"
    start_kingbase "$kingbase_log"
fi

printf "Running, please wait ......... "

if [ x"$os_platform" = x"sunos" ]; then
	export LD_LIBRARY_PATH_64="$oci_install_path":$LD_LIBRARY_PATH_64
else
export LD_LIBRARY_PATH="$oci_install_path":$LD_LIBRARY_PATH
fi

if [ x"$os_platform" = x"linux" -o x"$os_platform" = x"aix" -o x"$os_platform" = x"sunos" ]; then
	if test x"$use_logfile" = x"yes"; then
        ./DciTest $OCITest_opt >"$OCITest_log" 2>&1
    else
        ./DciTest $OCITest_opt 
	fi
else
	cp $proj_home/bin/ocikdb.dll .
	./Release/DciTest $OCITest_opt >"$OCITest_log" 2>&1
fi

if test x"$use_logfile" = x"yes"; then
    if [ -z "`cat \"$OCITest_log\" | grep \"ALL tests SUCCESS\"`" ]
    then 
        printf "FAILED\n"
    else
        printf "done\n"
    fi	
fi

if test x"$use_initkdb" = x"yes"; then
    stop_kingbase;
fi
if test x"$use_logfile" = x"yes"; then
    gather_log;
fi

printf "Cleaning enviroment .......... "
rm -f "$kingbase_home/config/sys_dblink.conf" \
      "$kingbase_home/config/sys_service.conf"
mv -f "$kingbase_home/config/sys_dblink.conf.bak" \
      "$kingbase_home/config/sys_dblink.conf"
mv -f "$kingbase_home/config/sys_service.conf.bak" \
      "$kingbase_home/config/sys_service.conf"
unset KINGBASE_CONFDIR
printf "done\n"

fix_odbc_config;
echo_test_over $test_what