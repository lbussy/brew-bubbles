#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/../../.." && pwd)"

asset_dir="${1:?Usage: $0 ASSET_DIRECTORY [OUTPUT_DIRECTORY]}"
output_dir="${2:-$repo_root/reconstructed-2.2.3}"

container_archive="brew-bubbles-pio-historical-5.1.0-linux-amd64.tar.gz"
cache_archive="brew-bubbles-2.2.3-offline-cache-linux-amd64.tar.gz"
image="brew-bubbles-pio-historical:5.1.0"

source_commit="3beff558e54862332a419fd5431d5ed59dd25a05"
expected_firmware_sha256="cfd41b647268a70c71090f621a44ad50d968a7cfba6dc5370d9849f59bf85f01"

asset_dir="$(cd "$asset_dir" && pwd)"
mkdir -p "$output_dir"
output_dir="$(cd "$output_dir" && pwd)"

if command -v shasum >/dev/null 2>&1; then
    sha256=(shasum -a 256)
elif command -v sha256sum >/dev/null 2>&1; then
    sha256=(sha256sum)
else
    echo "A SHA-256 utility (shasum or sha256sum) is required." >&2
    exit 1
fi

if [[ -n "${BB_PRESERVE_TMPDIR:-}" ]]; then
    temporary_root="$BB_PRESERVE_TMPDIR"
elif [[ "$(uname -s)" == "Darwin" ]]; then
    temporary_root=/private/tmp
else
    temporary_root=/tmp
fi

for archive in "$container_archive" "$cache_archive"; do
    if [[ ! -f "$asset_dir/$archive" ]]; then
        echo "Missing external asset: $asset_dir/$archive" >&2
        exit 1
    fi
done

echo "Verifying external assets..."
(
    cd "$asset_dir"
    "${sha256[@]}" -c "$script_dir/EXTERNAL-ASSETS.sha256"
)

if ! docker image inspect "$image" >/dev/null 2>&1; then
    echo "Loading preserved container image..."
    gzip -dc "$asset_dir/$container_archive" | docker load
fi

workspace="$(mktemp -d "$temporary_root/brew-bubbles-2.2.3.XXXXXX")"
trap 'rm -rf "$workspace"' EXIT

echo "Preparing historical source..."
git clone --quiet --no-hardlinks "$repo_root" "$workspace/project"
git -C "$workspace/project" checkout --quiet -B devel "$source_commit"
git -C "$workspace/project" tag --force preservation-build-base "$source_commit"
cp "$script_dir/platformio.ini" "$workspace/project/platformio.ini"

echo "Restoring offline packages and libraries..."
tar -xzf "$asset_dir/$cache_archive" -C "$workspace"

cp \
    "$repo_root/preserved/2.2.3/littlefs.bin" \
    "$workspace/canonical-littlefs.bin"

host_uid="$(id -u)"
host_gid="$(id -g)"

echo "Building with networking disabled..."
docker run --rm \
    --platform linux/amd64 \
    --network none \
    --user "$host_uid:$host_gid" \
    --env HOME=/work/home \
    --volume "$workspace:/work" \
    "$image" \
    bash -lc '
        set -euo pipefail
        cd /work/project

        pio run -e d1_mini
        pio run -e d1_mini -t buildfs

        actual_firmware_sha256="$(
            sha256sum .pio/build/d1_mini/firmware.bin |
            awk "{print \$1}"
        )"

        if [[ "$actual_firmware_sha256" != "'"$expected_firmware_sha256"'" ]]; then
            echo "Unexpected firmware checksum: $actual_firmware_sha256" >&2
            exit 1
        fi

        mkdir -p /work/fs-release /work/fs-rebuilt

        mklittlefs=/work/home/.platformio/packages/tool-mklittlefs/mklittlefs

        "$mklittlefs" -u \
            /work/fs-release \
            /work/canonical-littlefs.bin

        "$mklittlefs" -u \
            /work/fs-rebuilt \
            .pio/build/d1_mini/littlefs.bin

        (
            cd /work/fs-release
            find . -type f -print0 |
                sort -z |
                xargs -0 sha256sum
        ) > /work/fs-release.sha256

        (
            cd /work/fs-rebuilt
            find . -type f -print0 |
                sort -z |
                xargs -0 sha256sum
        ) > /work/fs-rebuilt.sha256

        diff -u /work/fs-release.sha256 /work/fs-rebuilt.sha256
    '

cp "$workspace/project/.pio/build/d1_mini/firmware.bin" "$output_dir/"
cp "$workspace/project/.pio/build/d1_mini/firmware.elf" "$output_dir/"
cp "$workspace/project/.pio/build/d1_mini/littlefs.bin" "$output_dir/"
cp "$workspace/fs-rebuilt.sha256" "$output_dir/littlefs-contents.sha256"

(
    cd "$output_dir"
    "${sha256[@]}" \
        firmware.bin \
        firmware.elf \
        littlefs.bin \
        littlefs-contents.sha256 \
        > SHA256SUMS
)

echo
echo "Offline reconstruction succeeded."
echo "Output: $output_dir"
cat "$output_dir/SHA256SUMS"
