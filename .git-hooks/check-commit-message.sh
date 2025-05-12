#!/bin/bash

commit_msg_file=$1
commit_msg=$(cat "$commit_msg_file")


if perl -e '
    binmode(STDIN, ":utf8");
    $/ = undef;
    $text = <>;
    if ($text =~ /[\x{4e00}-\x{9fff}]/) {
        exit(1);
    } else {
        exit(0);
    }' < "$commit_msg_file"
then
    exit 0
else
    echo "Error: Commit message contains Chinese characters."
    echo "Please use English only in commit messages."
    exit 1
fi 