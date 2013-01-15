git filter-branch -f --commit-filter '
        if [ "$GIT_COMMITTER_NAME" = "Randy Baumgarte" ];
        then
                GIT_COMMITTER_NAME="Randy Baumgarte";
                GIT_AUTHOR_NAME="Randy Baumgarte";
                GIT_COMMITTER_EMAIL="<randy@fbn.cx>";
                GIT_AUTHOR_EMAIL="<randy@fbn.cx>";
                git commit-tree "$@";
        else
                git commit-tree "$@";
        fi' HEAD
