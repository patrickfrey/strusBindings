# StrusBindings spec file

# Set distribution based on some OpenSuse and distribution macros
# this is only relevant when building on https://build.opensuse.org
###

%define with_php 1
%define with_python 0
%define with_java 0

%define rhel 0
%define rhel5 0
%define rhel6 0
%define rhel7 0
%if 0%{?rhel_version} >= 500 && 0%{?rhel_version} <= 599
%define dist rhel5
%define rhel 1
%define rhel5 1
%endif
%if 0%{?rhel_version} >= 600 && 0%{?rhel_version} <= 699
%define dist rhel6
%define rhel 1
%define rhel6 1
%endif
%if 0%{?rhel_version} >= 700 && 0%{?rhel_version} <= 799
%define dist rhel7
%define rhel 1
%define rhel7 1
%endif

%define centos 0
%define centos5 0
%define centos6 0
%define centos7 0
%if 0%{?centos_version} >= 500 && 0%{?centos_version} <= 599
%define dist centos5
%define centos 1
%define centos5 1
%endif
%if 0%{?centos_version} >= 600 && 0%{?centos_version} <= 699
%define dist centos6
%define centos 1
%define centos6 1
%endif
%if 0%{?centos_version} >= 700 && 0%{?centos_version} <= 799
%define dist centos7
%define centos 1
%define centos7 1
%endif

%define scilin 0
%define scilin5 0
%define scilin6 0
%define scilin7 0
%if 0%{?scilin_version} >= 500 && 0%{?scilin_version} <= 599
%define dist scilin5
%define scilin 1
%define scilin5 1
%endif
%if 0%{?scilin_version} >= 600 && 0%{?scilin_version} <= 699
%define dist scilin6
%define scilin 1
%define scilin6 1
%endif
%if 0%{?scilin_version} >= 700 && 0%{?scilin_version} <= 799
%define dist scilin7
%define scilin 1
%define scilin7 1
%endif

%define fedora 0
%define fc21 0
%define fc22 0
%if 0%{?fedora_version} == 21
%define dist fc21
%define fc21 1
%define fedora 1
%endif
%if 0%{?fedora_version} == 22
%define dist fc22
%define fc22 1
%define fedora 1
%endif

%define suse 0
%define osu131 0
%define osu132 0
%define osufactory 0
%if 0%{?suse_version} == 1310
%define dist osu131
%define osu131 1
%define suse 1
%endif
%if 0%{?suse_version} == 1320
%define dist osu132
%define osu132 1
%define suse 1
%endif
%if 0%{?suse_version} > 1320
%define dist osufactory
%define osufactory 1
%define suse 1
%endif

%define sles 0
%define sles11 0
%define sles12 0
%if 0%{?suse_version} == 1110
%define dist sle11
%define sles11 1
%define sles 1
%endif
%if 0%{?suse_version} == 1315 
%define dist sle12
%define sles12 1
%define sles 1
%endif

Summary: Language bindings for the strus text search engine
Name: strusbindings
%define main_version 0.1.7
Version: %{main_version}
Release: 0.1
License: GPLv3
Group: Development/Libraries/C++

Source: %{name}_%{main_version}.tar.gz

URL: http://project-strus.net

BuildRoot: %{_tmppath}/%{name}-root

# Installation directories
###

%if %{with_php}
%if %{suse} || %{sles}
%global phpdir php5
%global php_extdir %{_libdir}/%{phpdir}/extensions
%global php_inidir %{_sysconfdir}/%{phpdir}/conf.d
%global php_datadir %{_datadir}/%{phpdir}
%else
%global phpdir php
%global php_extdir %{_libdir}/%{phpdir}/modules
%global php_inidir %{_sysconfdir}/php.d
%global php_datadir %{_datadir}/%{phpdir}
%endif

%if %{suse} || %{sles} || %{rhel} || %{centos} || %{scilin}
%global php_inifile strus.ini
%else
%global php_inifile 40-strus.ini
%endif

%endif

# Build dependencies
###

# OBS doesn't install the minimal set of build tools automatically
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: cmake

# LinuxDistribution.cmake depends depends on the Linux release files in '/etc' or
# LSB files
%if %{rhel}
BuildRequires: redhat-release
%endif
%if %{centos}
BuildRequires: centos-release
%endif
%if %{scilin}
BuildRequires: sl-release
%endif
%if %{fedora} && !0%{?opensuse_bs}
BuildRequires: fedora-release
%endif
%if %{fedora} && 0%{?opensuse_bs}
BuildRequires: generic-release
%endif
%if %{suse}
BuildRequires: openSUSE-release
%endif
%if %{sles}
%if %{sles12}
#exists in sles12, missing on OBS!
#BuildRequires: sles-release
%else
BuildRequires: sles-release
%endif
%endif

# OBS has no repos containing some development libraries
%if %{rhel}
%define with_php 0
%endif

# Suse SL11 SP can't be distinguished, they have a way too old php
%if %{sles}
%if %{sles11}
%endif
%define with_php 0
%endif

%if %{rhel} || %{centos} || %{scilin} || %{fedora}
%if %{rhel5} || %{rhel6} || %{centos5} || %{centos6} || %{scilin5} || %{scilin6}
Requires: boost153 >= 1.53.0
BuildRequires: boost153-devel >= 1.53.0
%else
Requires: boost >= 1.53.0
Requires: boost-thread >= 1.53.0
Requires: boost-system >= 1.53.0
Requires: boost-date-time >= 1.53.0
BuildRequires: boost-devel
%endif
%endif

%if %{suse} || %{sles}
%if %{sles11}
Requires: boost153 >= 1.53.0
BuildRequires: boost153-devel >= 1.53.0
%endif
%if %{osu131}
Requires: libboost_thread1_53_0 >= 1.53.0
Requires: libboost_atomic1_53_0 >= 1.53.0
Requires: libboost_system1_53_0 >= 1.53.0
Requires: libboost_date_time1_53_0 >= 1.53.0
BuildRequires: boost-devel
# for some reason OBS doesn't pull in libboost_atomic1_53_0 automatically!?
BuildRequires: libboost_atomic1_53_0 >= 1.53.0
%endif
%if %{osu132} || %{sles12}
Requires: libboost_thread1_54_0 >= 1.54.0
Requires: libboost_atomic1_54_0 >= 1.54.0
Requires: libboost_system1_54_0 >= 1.54.0
Requires: libboost_date_time1_54_0 >= 1.54.0
BuildRequires: boost-devel
%endif
%if %{osufactory}
Requires: libboost_thread1_58_0 >= 1.58.0
Requires: libboost_atomic1_58_0 >= 1.58.0
Requires: libboost_system1_58_0 >= 1.58.0
Requires: libboost_date_time1_58_0 >= 1.58.0
BuildRequires: boost-devel
%endif
%endif

%if %{rhel} || %{centos} || %{scilin}
%if %{rhel7} || %{centos7} || %{scilin7}
BuildRequires: swig
%endif
%if %{rhel6} || %{centos6} || %{scilin6}
BuildRequires: swig2
%endif
%if %{rhel5} || %{centos5} || %{scilin5}
BuildRequires: swig2
%endif
%endif
%if %{fedora} || %{suse} || %{sles}
BuildRequires: swig
%endif

BuildRequires: strus-devel >= 0.1.6
BuildRequires: strusanalyzer-devel >= 0.1.6
BuildRequires: strusmodule-devel >= 0.1.5
BuildRequires: strusrpc-devel >= 0.1.9
BuildRequires: strus >= 0.1.6
BuildRequires: strusanalyzer >= 0.1.6
BuildRequires: strusmodule >= 0.1.5
BuildRequires: strusrpc >= 0.1.9
Requires: strus >= 0.1.6
Requires: strusanalyzer >= 0.1.6
Requires: strusmodule >= 0.1.5
Requires: strusrpc >= 0.1.9
%if %{rhel} || %{centos} || %{scilin} || %{fedora}
%if %{rhel5} || %{rhel6} || %{centos5} || %{centos6} || %{scilin5} || %{scilin6}
# Official Php is too old, so is EPEL, using Webtatic (TODO: have a SPEC-file switch
# choosing your php)
BuildRequires: php-devel
%else
BuildRequires: php-devel
%endif
%else
BuildRequires: php-devel
%endif

# Check if 'Distribution' is really set by OBS (as mentioned in bacula)
%if ! 0%{?opensuse_bs}
Distribution: %{dist}
%endif

Packager: Patrick Frey <patrickpfrey@yahoo.com>

%description
Library implementing the loading of dynalically loadable of a text search engine.

%if %{with_java}
%package java
Summary: strus Java language bindings
Group: Development/Libraries/Java

%description java
Language bindings for the strus text search enginge in Java

BuildRequires: java-1.8.0-openjdk-devel
Requires: java-1.8.0-openjdk

%endif

%if %{with_php}

%package php
Summary: strus Php language bindings
Group: Development/Libraries/Php

%description php
Language bindings for the strus text search engine in Php

%if %{rhel} || %{centos} || %{scilin} || %{fedora}
%if %{rhel5} || %{rhel6} || %{centos5} || %{centos6} || %{scilin5} || %{scilin6}
# Official Php is too old, so is EPEL, using Webtatic (TODO: have a SPEC-file switch
# choosing your php)
BuildRequires: php-devel
Requires: php
%else
BuildRequires: php-devel
Requires: php
%endif
%else
BuildRequires: php-devel
Requires: php
%endif

%endif

%if %{with_python}

%package python
Summary: strus Python language bindings
Group: Development/Libraries/Python

%description python
Language bindings for the strus text search engine in Python

BuildRequires: python-devel
Requires: python

%endif

%prep
%setup -n %{name}-%{main_version}

%build

# TODO: out-of-source build not possible currently!
#mkdir build
#cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DLIB_INSTALL_DIR=%{_lib} \
%if %{with_java}
	-DWITH_JAVA="YES" \
%endif
%if %{with_php}
	-DWITH_PHP="YES" \
%endif
%if %{with_python}
	-DWITH_PYTHON="YES" \
%endif
	.
# make -j2 breaks
#make %{?_smp_mflags}
make

%install

%if %{with_php}
cd swig/php
make DESTDIR=$RPM_BUILD_ROOT install
cd ../..

mkdir -p $RPM_BUILD_ROOT%{_datadir}/%{phpdir}
cp swig/php/strus.php $RPM_BUILD_ROOT/%{_datadir}/%{phpdir}/.

mkdir -p $RPM_BUILD_ROOT/%{php_inidir}
cat > $RPM_BUILD_ROOT/%{php_inidir}/%{php_inifile} <<EOF
; Enable strus extensions module
extension=strus_php.so
EOF

%endif
  
%clean
rm -rf $RPM_BUILD_ROOT

%check
# TODO: out-of-source build not possible!
#cd build

make test

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%if %{with_php}
%files php
%defattr( -, root, root )
%config(noreplace) %{php_inidir}/%{php_inifile}
%{php_extdir}/strus_php.so
%{_datadir}/%{phpdir}/strus.php
%endif

%if %{with_java}
%files java
%defattr( -, root, root )
# TODO: should not be in libdir but in jni subdir (at
# least on RHEL
%{_libdir}/strus/libstrus_java.so
%{_libdir}/strus/libstrus_java.so.0.1
%{_libdir}/strus/libstrus_java.so.0.1.7
%{_libdir}/strus/strus_api.jar
%endif

%changelog
* Fri Mar 20 2015 Patrick Frey <patrickpfrey@yahoo.com> 0.1.7-0.1
- preliminary release
