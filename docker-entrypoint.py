import sys
import os
import logging

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG, format='%(asctime)s : %(message)s')

argv = sys.argv

def os_system(cmd, info=1):
    cmd = cmd.strip('"').strip(",")
    msg = "> exec: {}".format(cmd)
    if info == 1:
        logging.info(msg)
    error = os.system(cmd)
    if error > 0:
        logging.info("run result: {}".format(error))
        sys.exit(1)

def change_conf(path):
    logging.info(path)
    rows = dict()
    for key in dict(os.environ).keys():
        value = os.getenv(key, None)
        if key[0:3] != "BD_":
            continue
        key = key[3:]
        #logging.info("%s=>%s", key, value)
        rows[key] = value
    if len(rows.keys()) > 0:
        f = open(path,"r")
        conf_content = f.read()
        f.close()
        new_conf = []
        for line in conf_content.split("\n"):
            line = line.split("=")
            if len(line) > 1:
                if line[0] in rows.keys():
                    new_conf.append("{0}={1}".format(line[0],rows[line[0]]))
                else:
                    new_conf.append("{0}={1}".format(line[0],line[1]))
        content = "\n".join(new_conf)
        #logging.info(content)
        f = open(path, "w")
        f.write(content)
        f.close()


print("==================>> CurrentVersion:"+os.getenv("VERSION"))
if len(argv) == 1:
    os_system("bash")
else:
    if argv[1] in [
        "push_server",
        "msg_server",
        "route_server",
        "http_msg_server",
        "db_proxy_server"
    ]:
        module = argv[1]
        server_name = "bd_{0}".format(module)
        server_conf_name = "ct_{0}.conf".format(module)
        project_dir = "/opt/bd-im/src"
        server_path = "{0}/{1}".format(project_dir, server_name)
        server_conf_path = "{0}/bd_{1}.conf".format(server_path, module)

        os_system("cp -a {0}/lib/log4cxx.properties {1}".format(project_dir, server_path))
        os_system("cp -a {0}/lib/libslog.so {1}".format(project_dir, server_path))
        os_system("cp -a {0}/lib/liblog4cxx.so* {1}".format(project_dir, server_path))

        if os.path.exists("/opt/conf/"):
            os_system("cp -a /opt/conf/{0} {1}".format(server_conf_name, server_path))

        if os.path.exists("/opt/conf/log4cxx.properties"):
            os_system("cp -a /opt/conf/log4cxx.properties {1}".format(server_conf_name, server_path))

        change_conf(server_conf_path)
        os_system("mkdir -p /opt/bd-im/log")
        os_system("cd {0} && ./{1}".format(server_path, server_name))
    else:
        os_system(argv[1])
