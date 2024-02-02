pipeline {
  agent any
  options {
    buildDiscarder logRotator(daysToKeepStr: '30', numToKeepStr: '30')
  }
  stages {
    stage('Build') {
      steps {
        sh '''#!/bin/bash
          chmod +x build.sh
          ./build.sh
        '''
      }
    }
    stage('Package') {
      steps {
        sh '''#!/bin/bash
          chmod +x package.sh
          ./package.sh
          mv package/riaps-timesync-amd64.deb .
          mv package/riaps-timesync-armhf.deb .
          mv package/riaps-timesync-arm64.deb .
        '''
      }
    }
    stage('Deploy') {
      when { buildingTag() }
      steps {
        // Install github-release cli tool to build directory
        sh 'GOPATH=$WORKSPACE/go go install github.com/aktau/github-release@latest'
        // Use GitHub OAuth token stored in 'github-token' credentials
        withCredentials([string(credentialsId: 'github-token', variable: 'GITHUB_TOKEN')]) {
          script {
            def user = 'riaps'
            def repo = 'riaps-timesync'
            def files = ['riaps-timesync-amd64.deb','riaps-timesync-armhf.deb','riaps-timesync-arm64.deb']
            // Create release on GitHub, if it doesn't already exist
            sh "${env.WORKSPACE}/go/bin/github-release release --user ${user} --repo ${repo} --tag ${env.TAG_NAME} --name ${env.TAG_NAME} --pre-release || true"
            // Iterate over artifacts and upload them
            for(int i = 0; i < files.size(); i++){
              sh "${env.WORKSPACE}/go/bin/github-release upload -R --user ${user} --repo ${repo} --tag ${env.TAG_NAME} --name ${files[i]} --file ${files[i]}"
            }
          }
        }
      }
    }
  }
  post {
    success {
      archiveArtifacts artifacts: '*.deb', fingerprint: true
    }
  }
}
