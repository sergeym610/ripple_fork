set -e

if [ -z "$DOCKER_EMAIL" -o -z "$DOCKER_USERNAME" -o -z "$DOCKER_PASSWORD" ];then
  echo "Docker credentials are not set. Can't login to docker, no containers will be pushed."
  exit 0
fi

if [ -n "$CIRCLE_PR_NUMBER" ]; then
  echo "Not pushing results of a pull request build."
  exit 0
fi

docker login -e $DOCKER_EMAIL -u $DOCKER_USERNAME -p $DOCKER_PASSWORD
docker push cbc/cbcd:$CIRCLE_SHA1
docker push cbc/cbcd:$CIRCLE_BRANCH
docker push cbc/cbcd:latest
