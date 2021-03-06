/*
Copyright (c) Facebook, Inc. and its affiliates.
All rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the root directory of this source tree.
*/

package main

import (
	"magma/lte/cloud/go/lte"
	"magma/lte/cloud/go/services/subscriberdb"
	"magma/lte/cloud/go/services/subscriberdb/obsidian/handlers"
	"magma/orc8r/cloud/go/obsidian"
	"magma/orc8r/cloud/go/service"

	"github.com/golang/glog"
)

// NOTE: subscriberdb service currently attaches no servicers.
// It still fulfills the service303 interface, and is kept around for future dev updates.
func main() {
	// Create the service
	srv, err := service.NewOrchestratorService(lte.ModuleName, subscriberdb.ServiceName)
	if err != nil {
		glog.Fatalf("Error creating service: %s", err)
	}
	obsidian.AttachHandlers(srv.EchoServer, handlers.GetHandlers())
	err = srv.Run()
	if err != nil {
		glog.Fatalf("Error while running service and echo server: %s", err)
	}
}
