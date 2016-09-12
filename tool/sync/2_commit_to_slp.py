#!/usr/bin/python
import os

def sync_web_engine():
    execute("git branch -D slp_master")
    execute("git branch slp_master slp/w/master")
    execute("git checkout slp_master")
    execute("git merge slp/upstream/tmp")
    #execute("cat tool/sync/slp_message.txt  | git commit --amend --file -")
    #execute("git push slp:magnolia/framework/web/web-widget-engine HEAD:refs/heads/w/master")
    execute("git checkout solis")

def sync_js():
    # May need to add the following repo is slp is unknown
    # git remote add slp ssh://<id>@slp-info.sec.samsung.net:29418/magnolia/framework/web/web-widget-engine-js
    os.chdir("third_party/escargot")
    execute("git branch -D slp_master")
    execute("git branch slp_master slp/w/master")
    execute("git checkout slp_master")
    execute("git merge slp/upstream/tmp")
    #execute("cat tool/sync/slp_message.txt  | git commit --amend --file -")
    #execute("git push slp:magnolia/framework/web/web-widget-js HEAD:refs/heads/w/master")
    execute("git checkout master")

def execute(cmd):
    print(cmd)
    os.system(cmd)

if __name__ == "__main__":
    sync_web_engine()
    sync_js()
