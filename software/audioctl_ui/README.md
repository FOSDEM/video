## audioctl_ui

Web UI that connects to [audioctl](../audioctl) for controlling the
rev-G and later [FOSDEM audio boards](../../hardware/pcbs/audio_board).

For earlier revisions see [this mixer UI](https://github.com/FOSDEM/infrastructure/blob/master/ansible/playbooks/roles/video-control-server/files/web/mixer.js)

### development

- Start [audioctl](../audioctl) (assuming it listens on port 8811)
- Run `API_URL='http://localhost:8811' ./build.sh serve`
- Navigate your browser to `http://localhost:5173`
