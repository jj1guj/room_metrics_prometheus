import requests
import yaml
import prometheus_client

if __name__ == "__main__":
    # Load configuration
    with open("client/config.yml", "r") as f:
        config = yaml.safe_load(f)

    for server_ip in config['server_ips']:
        url = f"http://{server_ip}"
        try:
            response = requests.get(url)
            values = response.json()
        except requests.exceptions.RequestException as e:
            print(f"Error connecting to {url}: {e}")
            continue
