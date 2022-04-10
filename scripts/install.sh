#!/bin/bash

# This script will connect to the target, copy the binary shared by NFS to the
# target home directory, create and enable a service to run the binary upon
# startup.
#
# This script assumes that:
# 	* the target (BeagleBone) to be connected to the host by Ethernet via USB,
# 	* that the folder ~/cmpt433/public/ in the host is being shared with the 
# 		target via NFS and mounted to /mnt/remote,
# 	* that the target username is "debian", 
#   * that the target password is temppwd, and
# 	* that the binary "recycler" is compiled and can be found in 
#   	~/cmpt433/public/myApps/recycler in the host.
# 
# Arguments:
#   1. The target IP address

# $1: Target IP
# $2: Target port
verify_ssh_connection () {
  timeout 1 bash -c "</dev/tcp/$1/$2"
  return $?
}

main () {
  TARGET_IP_ARG=$1

  APPNAME="recycler"
  CMPT433_DIR="${HOME}/cmpt433"
  BIN=$"${CMPT433_DIR}/public/myApps/${APPNAME}"

	## Verify if host has SSHPASS
	command -v sshpass &> /dev/null
	if [ $? -ne 0 ]
	then
		echo "sshpass is required for this script. Please, make sure it is \
installed prior to running this script."
		exit 1
	fi

	## Verify if binary exists 
  if [ ! -f "${BIN}" ]
  then
    >&2 echo "${BIN} does not exist. Have you built the binary?"
    exit 1
  fi

	## Verify connection to target via SSH
  TARGET_USERNAME="debian"
  TARGET_IP="192.168.7.2"
  SSH_PORT="22"

  if [ -n "${TARGET_IP_ARG}" ]
  then
    OLD_TARGET_IP=${TARGET_IP}
    TARGET_IP=${TARGET_IP_ARG}
    verify_ssh_connection ${TARGET_IP} ${SSH_PORT}
    if [ $? -ne 0 ]
    then
      >&2 echo "It was not possible to contact target through SSH. Ensure the \
IP address ${TARGET_IP} is the correct address and that the target is \
connected to the host."
      exit 1
    fi
  fi

  verify_ssh_connection ${TARGET_IP} ${SSH_PORT}
  if [ $? -ne 0 ]
  then
    TARGET_IP="192.168.6.2"
    verify_ssh_connection ${TARGET_IP} ${SSH_PORT}
    if [ $? -ne 0 ]
    then
      >&2 echo "It was not possible to contact target through SSH. Is the \
target IP address 192.168.7.2 or 192.168.6.2? Is it correctly \
connected to the host?"
      exit 1
    fi
  fi
	
  TARGET_CMPT433_PUBLIC="/mnt/remote"
  TARGET_BIN="${TARGET_CMPT433_PUBLIC}/myApps/${APPNAME}"
	TARGET_SYSTEMD_DIR="/lib/systemd/system"

	## Recycler service file
  read -r -d '' RECYCLER_SERVICE <<- EOM
	[Unit]
	Description=Recycler program startup

	[Service]
	User=root
	WorkingDirectory=/home/${TARGET_USERNAME}/
	ExecStart=/home/${TARGET_USERNAME}/recycler
	SyslogIdentifier=NFSMount

	[Install]
	WantedBy=multi-user.target
EOM

	PASSWORD="temppwd"

	## SSH commands to be executed.
  read -r -d '' COMMANDS <<- EOM
	if [ ! -f ${TARGET_BIN} ]
	then
		echo "Binary not found in target. Is the target NFS working and mounted to
		${TARGET_CMPT433_PUBLIC}?"
		exit 1
	fi

	echo "Copying target binary to home"
  cp ${TARGET_BIN} \${HOME}/${APPNAME}

	echo "Setting recycler service."
	echo ${PASSWORD} | sudo -S sh -c 'echo "${RECYCLER_SERVICE}" > ${TARGET_SYSTEMD_DIR}/${APPNAME}.service'
	echo ${PASSWORD} | sudo -S systemctl enable --now ${APPNAME}.service
  echo ""

	if [ $? -eq 0 ]
	then
		echo "${APPNAME} service enabled!"
	else
		echo "There was an error enabling the service!"
		exit 1
	fi

	echo "Exiting."
EOM

  sshpass -p ${PASSWORD} ssh -t ${TARGET_USERNAME}@${TARGET_IP} "${COMMANDS}"
}

main $1
