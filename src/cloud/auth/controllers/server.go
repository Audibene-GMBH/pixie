/*
 * Copyright 2018- The Pixie Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

package controllers

import (
	"context"

	"github.com/gofrs/uuid"

	"px.dev/pixie/src/cloud/auth/authenv"
)

// APIKeyMgr is the internal interface for managing API keys.
type APIKeyMgr interface {
	FetchOrgUserIDUsingAPIKey(ctx context.Context, key string) (uuid.UUID, uuid.UUID, error)
}

// AuthProvider interfaces the service we use for auth.
type AuthProvider interface {
	// GetUserIDFromToken returns the UserID for the particular auth token.
	GetUserIDFromToken(token string) (string, error)
	// GetUserInfo returns the UserInfo for the userID.
	GetUserInfo(userID string) (*UserInfo, error)
	// SetPLMetadata sets the pixielabs related metadata in the auth provider.
	SetPLMetadata(userID, plOrgID, plUserID string) error
}

// Server defines an gRPC server type.
type Server struct {
	env       authenv.AuthEnv
	a         AuthProvider
	apiKeyMgr APIKeyMgr
}

// NewServer creates GRPC handlers.
func NewServer(env authenv.AuthEnv, a AuthProvider, apiKeyMgr APIKeyMgr) (*Server, error) {
	return &Server{
		env:       env,
		a:         a,
		apiKeyMgr: apiKeyMgr,
	}, nil
}
