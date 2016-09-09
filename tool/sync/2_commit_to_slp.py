#!/usr/bin/python
import os

def run():
    # Merge slp/upstream/tmp with slp/w/master
    execute("git branch -D slp_master")
    execute("git branch slp_master slp/w/master")
    execute("git checkout slp_master")
    execute("git merge slp/upstream/tmp")
    execute("cat tool/sync/slp_message.txt  | git commit --amend --file -")
    execute("git push slp:magnolia/framework/web/web-widget-engine HEAD:refs/heads/w/master")
    execute("git checkout solis")

def execute(cmd):
    print(cmd)
    os.system(cmd)

if __name__ == "__main__":
    run()
