# StrusBindings spec file

# Set distribution based on some OpenSuse and distribution macros
# this is only relevant when building on https://build.opensuse.org
###

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
%define fc20 0
%define fc21 0
%if 0%{?fedora_version} == 20
%define dist fc20
%define fc20 1
%define fedora 1
%endif
%if 0%{?fedora_version} == 21
%define dist fc21
%define fc21 1
%define fedora 1
%endif

%define suse 0
%define osu122 0
%define osu123 0
%define osu131 0
%define osu132 0
%define osufactory 0
%if 0%{?suse_version} == 1220
%define dist osu122
%define osu122 1
%define suse 1
%endif
%if 0%{?suse_version} == 1230
%define dist osu123
%define osu123 1
%define suse 1
%endif
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
Version: 0.0.1
Release: 0.1
License: GPLv3
Group: Development/Libraries/C++

Source: %{name}_%{version}.tar.gz

URL: http://project-strus.net

BuildRoot: %{_tmppath}/%{name}-root

# Build dependencies
###

# OBS doesn't install the minimal set of build tools automatically
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: cmake

%if %{rhel} || %{centos} || %{scilin} || %{fedora}
%if %{rhel5} || %{centos5}
Requires: boost148 >= 1.48
BuildRequires: boost148-devel >= 1.48
%else
Requires: boost >= 1.48
Requires: boost-thread >= 1.48
Requires: boost-system >= 1.48
Requires: boost-date_time >= 1.48
BuildRequires: boost-devel
%endif
%endif
%if %{suse} || %{sles}
BuildRequires: boost-devel
%if %{osu122} || %{osu123} || %{sles11} || %{sles12}
Requires: libboost_thread1_49_0 >= 1.49.0
Requires: libboost_system1_49_0 >= 1.49.0
Requires: libboost_date_time1_49_0 >= 1.49.0
%endif
%if %{osu131}
Requires: libboost_thread1_53_0 >= 1.53.0
Requires: libboost_system1_53_0 >= 1.53.0
Requires: libboost_date_time1_53_0 >= 1.53.0
%endif
%endif

# Check if 'Distribution' is really set by OBS (as mentioned in bacula)
%if ! 0%{?opensuse_bs}
Distribution: %{dist}
%endif

Packager: Patrick Frey <patrickpfrey@yahoo.com>

%description
Library implementing the loading of dynalically loadable of a text search engine.

%package devel
Summary: strus language bindings
Group: Development/Libraries/C++

%description devel
Language bindings for the strus text search engine

Requires: %{name} >= %{version}-%{release}

%prep
%setup

%build

mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DLIB_INSTALL_DIR=%{_libdir} ..
make %{?_smp_mflags}

%install

cd build
make DESTDIR=$RPM_BUILD_ROOT install

# TODO: avoid building this stuff in cmake. how?
rm -rf $RPM_BUILD_ROOT%{_libdir}/debug
rm -rf $RPM_BUILD_ROOT%{_prefix}/src/debug

%clean
rm -rf $RPM_BUILD_ROOT

%check
cd build
make test

%files
%defattr( -, root, root )
%dir %{_libdir}/%{name}
%{_libdir}/%{name}/libstrus_bindings.so.0.0
%{_libdir}/%{name}/libstrus_bindings.so.0.0.1

%files devel
%{_libdir}/%{name}/libstrus_bindings.so
%dir %{_includedir}/%{name}
%{_includedir}/%{name}/*.hpp

%changelog
* Fri Mar 20 2015 Patrick Frey <patrickpfrey@yahoo.com> 0.0.1-0.1
- preliminary release
