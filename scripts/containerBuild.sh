#!/usr/bin/env bash
set -euo pipefail

# Resolve the repo root as the directory one level up from this script,
# so the build works no matter where it's invoked from.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

IMAGE_NAME="${IMAGE_NAME:-linux-low-level-protocols}"
IMAGE_TAG="${IMAGE_TAG:-latest}"
CONTAINERFILE="${REPO_ROOT}/Containerfile"

echo "Building image: ${IMAGE_NAME}:${IMAGE_TAG}"
echo "Containerfile:  ${CONTAINERFILE}"
echo "Build context:  ${REPO_ROOT}"

podman build \
  -t "${IMAGE_NAME}:${IMAGE_TAG}" \
  -f "${CONTAINERFILE}" \
  "${REPO_ROOT}"

echo "Done: ${IMAGE_NAME}:${IMAGE_TAG}"
