# admx

Linux tools and scripts usefull to admin and manage machines. Started to be a practise bash scripts, becoming a base framework for administration and automate several tasks through many (a lot) servers.
(Scripts submitted to SAST tools)

# Commands

## dnf

For Red Hat like distros.

- dnf-remote.sh
- dnf-remote-bulk.sh

Scripts to execute dnf remotely using ssh. Bulk means that an input file for hosts list is used to execute in several machines.

"dnf-repo*.sh" are scripts to simplify the execute of dnf repoquery and format the output. 

## fedora remove old kernels
(Maybe applicable for Red Hat like distro)

Simple. Remove old kernels.

## find related to security

- find-dirs-write-others.sh
- find-files-write-others.sh

Find directories or files with other write permissions.

- find-unsecure-others.sh

Find files that can contain "danger" words and with other permissions. Danger words like "password", "secret", etc ...

## firewall

Firewall scripts to add or remove sources. Useful to manage different network segments with different firewall profiles.

## IPA

IPA scripts to manage ipa servers and simplify tasks.

Tasks such as:

- Add sudo rule.
- Add user to sudo rule.
- Update IPA CA certificate in the system.
- Add, modify and delete DNS records.
- Add user.
- Unlock user.
- Check user status.
- Change user password.
- Request certificate for host.
- Request certificate for an http service.
- Request certificate for a web host (including virtual host (vh)).

## loginctl

Script to execute loginctl in several machines and check who is logged (loginctl-bulk.sh).
loginctl-show-session.sh, shows the parameters of current session.

## lsblk

Scripts to simplify the use of lsblk command. 

- lsblk-view-basic.sh
- lsblk-view.sh

More beautiful output for lsblk command.

## nmcli related scripts

A big set of nmcli related scripts. nmcli command has a lot of options and uses, making this command sometime painful for some basic tasks.

So this agile the use for:

- Add bridge connection.
- Add wired connection.
- Show connections.
- Show devices.
- Show hostname and permissions.
- Check network connectivity.
- Show radio devices.
- Set DNS for connection.
- Set gateway.
- Set hostname.
- Set ipv4 address.
- Set methods.
- Set MTU.

## rsystemctl

Execute of systemctl commands in remte machines.

### rsystemctl-bulk-list-units-failed.sh

Check the failed services in remote machines.

### rsystemctl-bulk.sh

Execute systemctl in remote machines.

## smb

Set selinux for a samba (smb) share.
For security reasons, this is a concern.

# Format for input of hosts list
(used also in https://github.com/jchurrocarvalho/utilx project)

This format is now used in scripts for remote commands execution such as rcmd-bulk.sh and scp-bulk*.sh.

The format of the files used as input for such commands are simple as text files with 3 simples rules:

- Prefix "P" to indicate the port. Any number after "P" until "H" or another char (not number) will be assumed as the port where ssh is listening.
- Prefix "H". After it is a string until end of line or until space char to indicate the host.
- If prefix "P" is not present in the line, the ssh default port is assumed (22).

I know this is a very basic "format", but it was useful to rapidly build a way for automatization of several maintenance tasks as security updates, certificates updates, log monitoring, etc …
So think this as a starting point. In the **TODO** list is included in future release, to adopt a json format somehow standard for hosts list.

Products to see:

- glpi
- wazuh
- otrs
- Other CMDB tool, or asset manager

# TODO
(For now ...)

- Introduce the json format for reading host list.
- Update dnf scripts to dnf5.

## C++ ACL sync tool

A new C++ (Clang + CMake) tool is available in `cpp/acl_sync` to copy owner user/group mode permissions to ACL entries on files or directories. See `cpp/acl_sync/README.md` for build, test, symbolic execution, and VS Code usage.
