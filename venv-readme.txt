To get the requirements that a venv needs, use "pip freeze" while in the venv.
Put the output from this command in "requirements.txt"

When a new machine clones the repo, they can setup their venv using a single command:
"virtualenv .env && source .env/bin/activate && pip install -r requirements.txt"