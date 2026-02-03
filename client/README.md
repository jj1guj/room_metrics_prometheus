## Set systemd

`/etc/systemd/system/room-metrics.service`:

```
[Unit]
Description=Room metrics exporter

[Service]
Type=oneshot
ExecStart=/usr/bin/python /path/to/room_metrics_prometheus/client/main.py --config /path/to/room_metrics_prometheus/client/config.yml
```

`/etc/systemd/system/room-metrics.timer`:

```
[Unit]
Description=Run room metrics exporter every 5s

[Timer]
OnBootSec=5s
OnUnitActiveSec=5s
Unit=room-metrics.service

[Install]
WantedBy=timers.target
```
