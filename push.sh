scp -r -i ssh-key-Robespierre.key ./src ubuntu@"$RobAddr":~/Robespierre/src

ssh -i ssh-key-Robespierre.key ubuntu@"$RobAddr" <<EOF
cd ~/Robespierre
make
sudo systemctl restart robespierre
EOF
