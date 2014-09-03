#ifndef _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_
#define _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_

#include "jstring.hpp"
#include "jhash.hpp"

static const int   DEFAULT_TIMEOUT(5*1000);
static const int   DALI_CHECK_TIMEOUT(DEFAULT_TIMEOUT);
static const int   SASHA_CHECK_TIMEOUT(DEFAULT_TIMEOUT);

const int DISK_SPACE_PERCENTAGE_WARNING     = 15;
const int DISK_SPACE_PERCENTAGE_CRITICAL    = 8;
const int USER_NUMBER_WARNING               = 5;
const int USER_NUMBER_CRITICAL              = 10;
const int TOTAL_PROCS_WARNING               = 350;
const int TOTAL_PROCS_CRITICAL              = 500;
const float SYSTEM_LOAD1_WARN               = 5.0;
const float SYSTEM_LOAD5_WARN               = 4.0;
const float SYSTEM_LOAD15_WARN              = 3.0;
const float SYSTEM_LOAD1_CRITICAL           = 10.0;
const float SYSTEM_LOAD5_CRITICAL           = 6.0;
const float SYSTEM_LOAD15_CRITICAL          = 4.0;

static const char *P_CHECK_NRPE("check_nrpe!");
static const char *P_CHECK_NRPE_1_ARG("check_nrpe_1_arg!");
static const char *P_CHECK_NRPE_2_ARG("check_nrpe_2_arg!");

static const char *P_CONFIGGEN_PARAM_LIST(" -list ");
static const char *P_CONFIGGEN_PARAM_LIST_ALL(" -listall ");
static const char *P_CONFIGGEN_PARAM_ENVIRONMENT(" -env ");
static const char *P_CONFIGGEN_PARAM_MACHINES(" -machines ");
static const char *P_CONFIGGEN_PARAM_LIST_ESP_SERVICES(" -listespservices ");
static const char *P_CHECK_ESP_SERVICE("check_esp_service_");
static const char *P_CHECK_SSH_SERVICE("check_ssh_hpcc_machine");
static const char *P_CHECK_DALI("check_dali");
static const char *P_CHECK_SASHA("check_sasha");
static const char *P_CHECK_ROXIE("check_roxie");
static const char *P_CHECK_DAFILESRV("check_dafilesrv");
static const char *P_CHECK_PROCS("check_procs");
static const char *P_CHECK_DISK_SPACE("check_all_disks");
static const char *P_CHECK_USERS("check_users");
static const char *P_CHECK_LOAD("check_load");
static const char *P_HTTP("http");
static const char *P_HTTPS("https");
static const char *P_BY_TYPE(" -t ");

struct NodeName
{
    StringBuffer strHostName;
    StringBuffer strHostAlias;
};

typedef MapStringTo<NodeName> MapIPtoNode;

/*
define host {
        use                     generic-host  ; Name of host template to use
        host_name               server02
        alias                   Server 02
        address                 172.18.100.101
}
*/
static const char *P_NAGIOS_HOST_CONFIG_1("define host{\n\
    use         generic-host\n\
    host_name   ");
static const char *P_NAGIOS_HOST_CONFIG_2("\n\
    alias       ");
static const char *P_NAGIOS_HOST_CONFIG_3("\n\
    address     ");
static const char *P_NAGIOS_HOST_CONFIG_4("\n\
}\n");


/*
define service {
          use                             generic-service
          host_name                       server02
          service_description             DNS
          check_command                   check_dns!172.18.100.101
}
*/
static const char *P_NAGIOS_SERVICE_CONFIG_1("define service {\n\
    use                  generic-service\n\
    host_name            ");
static const char *P_NAGIOS_SERVICE_CONFIG_2("\n\
    service_description  ");
static const char *P_NAGIOS_SERVICE_CONFIG_3("\n\
    check_command        ");
static const char *P_NAGIOS_SEPERATOR("!");  // NOT SURE IF THIS IS USEFUL
static const char *P_NAGIOS_SERVICE_CONFIG_5("\n\
}\n");

/*
define service {
        hostgroup_name        mysql-servers
        service_description   MySQL
        check_command         check_mysql_cmdlinecred!nagios!secret!$HOSTADDRESS
        use                   generic-service
        notification_interval 0 ; set > 0 if you want to be renotified
}
*/
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_1("define service{\n\
    hostgroup_name       ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_2("\n\
    service_description  ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_3("\n\
    check_command        ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_4("\n\
    use                  generic-service\n\
    notification_interval  0\n\
}\n");


/*
define hostgroup {
        hostgroup_name  mysql-servers
                alias           MySQL servers
                members         localhost, server02
        }
*/

static const char *P_NAGIOS_HOSTS_GROUP_CONFIG_1("define hostgroup{\n\
    hostgroup_name      ");
static const char *P_NAGIOS_HOSTS_GROUP_CONFIG_2("\n\
    alias               ");
static const char *P_NAGIOS_HOSTS_GROUP_CONFIG_3("\n\
    members             ");
static const char *P_NAGIOS_HOSTS_GROUP_CONFIG_4("\n\
}\n");

#endif // _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_
