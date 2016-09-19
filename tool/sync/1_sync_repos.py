#!/usr/bin/python
import os

def sync_web_engine():
    # 0. start at solis
    execute("git checkout solis")
    execute("make x64.exe.release -j")

    # 1. Pushing solis to spin
    # git remote add spin ssh://{}@165.213.149.170:29418/framework/web/wearable/web-widget-engine"
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

def sync_js():
    os.chdir("third_party/escargot")
    execute("git checkout master")
    execute("git branch -D tizen_2.3")
    execute("git checkout -b tizen_2.3")

    execute("git push spin:framework/web/wearable/web-widget-js HEAD:refs/heads/tizen_2.3")
    execute("git push slp:magnolia/framework/web/web-widget-js HEAD:refs/heads/upstream/tmp")
    execute("git push review:profile/wearable/platform/framework/web/web-widget-js HEAD:refs/heads/tizen")

    execute("git checkout master")

def execute(cmd):
    print(cmd)
    os.system(cmd)

if __name__ == "__main__":
    sync_web_engine()
    sync_js()
