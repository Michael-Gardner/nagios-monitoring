################################################################################
#    HPCC SYSTEMS software Copyright (C) 2014 HPCC Systems.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
################################################################################


# Component: check_roxie
#####################################################
# Description:
# ------------
#    Cmake check_roxie
#####################################################
project( check_roxie )

IF ( "${packageManagement}" STREQUAL "RPM" )
        IF ( "${ARCH64BIT}" EQUAL 1 )
                install ( PROGRAMS check_roxie DESTINATION /usr/lib64/nagios/plugins/ )
        ELSE ()
                install ( PROGRAMS check_roxie DESTINATION /usr/lib/nagios/plugins/ )
        ENDIF ()
ELSE ()
        install ( PROGRAMS check_roxie DESTINATION /usr/lib/nagios/plugins/ )
ENDIF ()
