#!/usr/bin/python
import os

# vi .ssh/config
# Host spin
#  HostName 165.213.149.170
#  User duddlf.choi
#  Port 29418
#  IdentityFile /home/ryanc/yichoi_ssh/id_rsa
#  IdentitiesOnly yes
# Host slp
#   HostName slp-info.sec.samsung.net
#   User duddlf.choi
#   Port 29418
#   IdentityFile /home/myuserId/yichoi_ssh/id_rsa
#   IdentitiesOnly yes
# Host review
#   HostName review.tizen.org
#   User yichoi
#   Port 29418
#   IdentityFile /home/myuserId/yichoi_ssh/id_rsa
#   IdentitiesOnly yes

def run():
    # 0. start at solis
    execute("git checkout solis")
    execute("make x64.exe.release -j")

    # 1. Pushing solis to spin
    #execute("git remote add spin ssh://{}@165.213.149.170:29418/framework/web/wearable/web-widget-engine".format(ID))
    execute("git branch -D tizen_2.3")
    execute("git checkout -b tizen_2.3")

    # Disabling prepush
    execute("rm -f .git/hooks/pre-push")
    execute("git push spin:framework/web/wearable/web-widget-engine HEAD:refs/heads/tizen_2.3 --force")

    # 2. Sync slp with solis
    execute("git push slp:magnolia/framework/web/web-widget-engine HEAD:refs/heads/upstream/tmp")

    # 3. sync review.tizen.org with solis
    execute("git push review:profile/wearable/platform/framework/web/web-widget-engine HEAD:refs/heads/tizen")

    execute("git stash")
    execute("git checkout solis")

def execute(cmd):
    print(cmd)
    os.system(cmd)

if __name__ == "__main__":
    run()
