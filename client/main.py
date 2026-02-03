import argparse
import os
import prometheus_client
import requests
import yaml

if __name__ == "__main__":
    args = argparse.ArgumentParser()
    args.add_argument('--config', type=str, default='config.yml', help='Path to configuration file')
    args = args.parse_args()

    # Load configuration
    with open(args.config, "r") as f:
        config = yaml.safe_load(f)

    for server_ip in config['server_ips']:
        url = f"http://{server_ip}"
        try:
            response = requests.get(url)
            values = response.json()

            room_name = values['label']
            save_dir = os.path.join(config['prometheus_metrics_path'], room_name)
            if not os.path.exists(save_dir):
                os.makedirs(save_dir)
            save_path = os.path.join(save_dir, 'metrics.prom')

            # set temp and hum metrics
            temp = values['temp']
            hum = values['hum']
            temp_gauge = prometheus_client.Gauge(f'{room_name}_temp', 'Gauge')
            hum_gauge = prometheus_client.Gauge(f'{room_name}_hum', 'Gauge')

            temp_gauge.set(temp)
            hum_gauge.set(hum)

            # set press metric if available
            if 'press' in values:
                press = values['press']
                press_gauge = prometheus_client.Gauge(f'{room_name}_press', 'Gauge')
                press_gauge.set(press)

            prometheus_client.write_to_textfile(save_path, prometheus_client.REGISTRY)
        except requests.exceptions.RequestException as e:
            if response is not None:
                print(f"Error connecting to {url}: Status code {response.status_code}")
            else:
                print(f"Error connecting to {url}: {e}")
            continue
