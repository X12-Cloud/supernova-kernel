#!/bin/bash
git add .
# read -p "Commit message: " msg

# Get the name of the branch you're currently on
current_branch=$(git rev-parse --abbrev-ref HEAD)

git commit -m "$1"
git push origin "$current_branch"
