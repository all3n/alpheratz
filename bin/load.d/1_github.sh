function get_latest_archive(){
    local NAME="$1"
    local FILTER="$2"
    curl --silent "https://api.github.com/repos/${NAME}/releases/latest" |grep browser_download_url|awk -F'"' '{print $4}'|grep -i "${FILTER}"
}

