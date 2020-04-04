// Code generated for package schema by go-bindata DO NOT EDIT. (@generated)
// sources:
// schema.graphql
package schema

import (
	"bytes"
	"compress/gzip"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
	"time"
)

func bindataRead(data []byte, name string) ([]byte, error) {
	gz, err := gzip.NewReader(bytes.NewBuffer(data))
	if err != nil {
		return nil, fmt.Errorf("Read %q: %v", name, err)
	}

	var buf bytes.Buffer
	_, err = io.Copy(&buf, gz)
	clErr := gz.Close()

	if err != nil {
		return nil, fmt.Errorf("Read %q: %v", name, err)
	}
	if clErr != nil {
		return nil, err
	}

	return buf.Bytes(), nil
}

type asset struct {
	bytes []byte
	info  os.FileInfo
}

type bindataFileInfo struct {
	name    string
	size    int64
	mode    os.FileMode
	modTime time.Time
}

// Name return file name
func (fi bindataFileInfo) Name() string {
	return fi.name
}

// Size return file size
func (fi bindataFileInfo) Size() int64 {
	return fi.size
}

// Mode return file mode
func (fi bindataFileInfo) Mode() os.FileMode {
	return fi.mode
}

// Mode return file modify time
func (fi bindataFileInfo) ModTime() time.Time {
	return fi.modTime
}

// IsDir return file whether a directory
func (fi bindataFileInfo) IsDir() bool {
	return fi.mode&os.ModeDir != 0
}

// Sys return file is sys mode
func (fi bindataFileInfo) Sys() interface{} {
	return nil
}

var _schemaGraphql = []byte("\x1f\x8b\x08\x00\x00\x00\x00\x00\x00\xff\x9c\x56\x4f\x6f\xdb\x3a\x12\xbf\xeb\x53\x4c\x90\x43\x13\x20\xf1\x61\xb1\xdb\x83\x6e\x5e\xdb\xbb\x15\x9a\x38\xd9\xd8\x49\xb7\xaf\x28\x82\x31\x39\xb6\x08\x4b\xa4\x4a\x8e\x9c\xf8\x3d\xf4\xbb\x3f\x90\x94\x64\xc9\x71\x5f\xd1\x77\x12\x45\xce\x9f\xdf\xfc\x9f\x73\x58\xe6\xca\xc1\x5a\x15\x04\x92\x9c\xb0\x6a\x45\x0e\x38\x27\x70\x22\xa7\x12\x61\x6d\x4d\x19\xfe\xc7\xf7\x19\x38\xb2\x3b\x25\x68\x94\x9c\x27\xe7\x90\xf1\x3b\x07\xda\x30\x28\x49\x58\x5c\xc1\xaa\x66\x78\x21\xd0\x44\x12\xd8\x40\x89\xba\xc6\xa2\xd8\xc3\x86\x34\x59\x64\x02\xde\x57\xe4\x60\x6d\x6c\x90\xb7\xdc\x57\xb4\x10\x56\x55\x0c\x8f\x59\x72\x0e\x2f\x39\x69\xe0\x0e\x8c\x72\x50\x57\x12\x99\xe4\x28\x42\x14\xa8\x61\x45\x20\x8d\x26\x58\xed\xc1\xd6\x5a\x2b\xbd\x49\x93\x73\x80\x8d\xc5\x2a\xff\x56\x5c\x47\xc8\xd7\x41\x4f\x94\xdc\xea\xbe\x66\xd7\x18\x34\x6a\x88\xe1\xfa\xda\xd4\x5c\xd5\xdc\xde\xcb\x11\xbb\x00\x43\x89\x1c\x5e\x54\x51\xf4\x80\xe7\x04\x0d\xb1\x97\x1d\x01\x72\x8e\x1c\xe9\x56\x04\x95\x12\x5b\x92\x50\x57\x1e\x9a\x27\x7f\xcc\x46\x49\xe3\xdb\x9e\xfc\xc0\xe9\xc0\xe5\xa6\x2e\x24\xd0\xab\x72\x0c\x4a\x47\x77\x63\x49\x20\x95\x25\xc1\xc6\xee\x01\xfb\x41\xe8\x30\x7b\xf6\x51\x92\x34\xa1\xf9\x23\x01\xf8\x56\x93\xdd\xa7\xf0\x3f\xff\x49\x00\xca\x9a\x91\x95\xd1\x29\xdc\x36\xa7\xe4\x7b\x92\x04\xd0\x8f\x8e\x6c\xa6\xd7\x26\xb0\x29\x99\x42\x36\x3d\x4b\x00\x34\x96\x94\xc2\x82\xad\xd2\x1b\xff\x4f\x25\xaa\xa2\x7f\x51\x29\xc1\xb5\x1d\xd0\x18\xbb\x99\x0f\xd8\xbe\x27\x09\xe9\xba\x84\xb1\x65\xb5\x46\xc1\x3e\xb6\x41\x0f\xc0\x78\xf9\xfc\x38\xff\x38\xbf\xfb\x34\x6f\x7f\x6f\xb2\xf9\xe3\xff\x9f\xc7\xb7\xd3\xf7\xff\x6c\xaf\xa6\xe3\x87\x4f\xd9\x7c\x78\x37\xb9\x9b\x2f\xc7\xd9\x7c\xf6\xf0\xbc\x98\x2d\x9f\x3f\x8f\x6f\x6f\x16\xa7\x9f\xfa\xf2\x3a\x20\x35\x1b\x61\xca\xaa\x20\xa6\xb1\xf0\x7e\xe8\x20\x8d\x07\x88\xce\x61\xac\x81\xa4\x62\xc0\x40\x06\x46\x88\xda\x3a\x50\x6b\x40\xa8\x1d\x59\xc8\xd1\x41\x69\xa4\x5a\x2b\x9f\xd7\x39\x81\xd2\x21\x11\xe8\x95\x7d\xb0\x95\x76\x64\x59\xe9\x0d\x18\x0b\x92\x0a\x0a\x67\x91\xa3\x45\xc1\x64\xdd\x28\x28\x09\x89\xa0\xb4\x28\x6a\xe9\xcb\x6b\x5f\x05\x86\x18\xf9\x2d\xed\x57\x06\xad\x04\xd4\x12\x2a\x74\x51\x80\x29\x4b\xd4\x32\xb0\x7b\xc4\xb3\x69\xb6\x8c\x70\xc1\x51\x41\xe2\x80\x57\x17\xfb\xd3\xa0\x45\x6e\x1c\x69\x40\x0d\xd8\xf3\x06\xb8\x7a\xb3\x21\xe7\x79\x47\x2d\x2c\xa9\x04\xb2\xc7\x65\x82\x0a\x0f\x6a\xc0\x12\x52\x5d\x71\x9b\xb7\xa5\xd9\xc5\x9a\xf0\xaa\xde\x39\xf0\xba\x7d\x51\x9b\x70\xa9\xbd\x63\xb0\xaa\xac\xa9\xac\x0a\xd5\x83\xab\xd6\x8a\xc5\xec\x66\x36\x59\x9e\x8c\xd2\x4c\xb3\xe2\xfd\x47\xa5\x65\x8c\xd2\xec\x63\x2f\x4a\xfe\xef\xfe\x6e\xda\x9c\x16\x4f\x93\xf6\x34\x79\xc8\xee\x97\xcd\xcf\x7c\x7c\x3b\x5b\xdc\x8f\x27\xb3\x2e\xe5\x43\x55\x04\x71\x1e\x69\xda\x95\x80\xcf\x61\x51\xd4\x8e\xfd\xe5\x24\x1e\x8e\xee\x27\x46\x6b\x12\xb1\x94\x26\xc7\x57\x07\x5a\xd5\x66\xfc\x05\xf6\x52\x3f\x1d\x14\xc2\x65\x0a\x93\x9b\xac\xbd\xf1\x7c\x2d\xad\xeb\xb8\xfa\xe5\x74\x79\x60\x77\xad\xa6\x7e\x38\x2e\x42\x02\xb6\xd4\x57\x8d\xfb\xef\x8d\x4b\x21\xd3\x7c\xd5\x24\x46\xfa\x83\x1a\xb8\x1c\x3e\x3c\x90\xab\x0b\x3e\x4b\x42\xe0\x63\x3b\x2e\x37\x16\x48\xcb\xca\x28\xcd\xee\x0a\x2c\xad\x29\x04\x57\x1a\xe1\x93\x05\x44\x61\x6a\x89\x95\x1a\x55\xd6\x84\x8c\x29\xd4\x8e\x9e\x14\xbd\xb8\x14\xbe\xdc\x34\xe7\x5b\x62\x94\xc8\x78\xf6\xf5\xac\x47\x31\x31\x9a\x49\xb3\xbb\x68\xfa\xcf\x65\x0a\x37\x47\x4f\x9e\x3c\x36\x6f\x2f\x2e\x22\x1a\x0a\x8b\xaf\x27\x44\x2d\x06\x0f\x67\x5d\x1a\xbc\xb5\x37\xe4\x84\xaf\x4a\xf2\xe3\xa8\x44\x66\x92\x4d\x5d\x2b\xd7\x2b\x72\xd7\xb8\x38\x0e\x05\x5f\x54\x2b\x22\x0d\x15\x5a\x47\xb2\x6d\xf5\xc3\x52\x31\x5d\x3d\xc5\x5a\xc2\xd5\x82\x4d\x05\x95\x71\xca\x87\x20\x14\x74\xa7\x33\xeb\x47\x32\xd0\x7f\xca\x89\x73\xb2\x6f\x30\x78\x5c\x08\x3b\x2c\x94\xbc\x02\x7a\x25\x51\x33\xae\x0a\x6a\xfb\x84\x97\xaa\xdc\xac\xbb\x4f\xe1\xdf\xc6\x14\x84\x3a\xf6\x8c\xa2\xe8\x95\x7d\x1c\xc1\x84\x22\x07\xb3\x0e\x8a\x1a\x90\x01\x9b\x3f\x1f\x48\x53\xf8\xb2\xec\x5f\x7c\xed\x9c\x3a\xb8\xee\xf9\x53\x69\x49\xaf\x3d\xc1\xb1\x79\x70\x4e\x8e\x06\x18\xd0\x06\xdf\x37\x2a\x33\xcf\x15\xf2\x77\xe0\x85\xd8\xea\xbc\xf9\xd8\x63\x6e\x56\x08\x1f\x29\x5c\x35\x0a\xc3\x20\x2e\x71\x1b\x1b\x53\xe3\x95\x9e\xa3\xbc\x9e\xc3\xdf\x78\xcd\x64\x17\x41\x78\xdf\x53\x6e\x60\x78\x3f\x6d\x4e\x79\xe0\xf4\x7b\x70\xc5\x56\x69\x99\xfe\xa0\xbd\x1d\xcd\xdc\x04\x9a\xbd\xab\x8a\x55\xdb\xdc\xb6\x2d\xb2\xe9\x3d\x0b\x46\xae\x5d\xcf\xcd\x92\xd6\xe8\x13\xd9\xb1\x6f\xb1\x6a\xed\x17\xb1\xbc\xc9\x93\xad\x36\x2f\xda\x1b\xfc\xf4\xdb\xf3\x62\x38\xec\x3c\x6b\xc3\xe2\x20\x27\x2c\x38\xdf\x7b\xee\x9c\xd0\xf2\x8a\x90\x63\x60\x2c\x09\x52\x3b\x92\x7e\x44\x59\xda\xd4\x05\x5a\x50\x9a\xc9\xee\xb0\x70\x61\x4e\x71\x1e\xf3\xbb\x69\x97\x5e\x9c\x25\x57\x19\x2d\x3d\x08\x36\x60\xe9\x5b\x4d\x8e\xdd\x01\xc7\x87\xd9\xf8\x66\xf9\xe1\xf3\x11\x8e\xb8\x69\x99\xd0\x60\x94\x13\xb1\xc7\xfa\x6a\x8c\x19\xf4\xdf\x87\xfb\x09\x88\xae\xf3\xc2\xca\x12\x6e\xdd\x28\x08\xc8\x4d\x45\xb1\x5e\x91\xbb\xc1\xd5\x02\x0a\x72\x85\x29\x09\x56\x28\xb6\x7e\x4c\x2a\x4d\x01\xba\x25\x57\x97\x3e\x51\xa1\x41\x14\x91\x1c\x80\x4e\xb3\xc5\xe4\x6e\x3e\x9f\x4d\x96\xb3\x69\x17\xed\x27\xf5\xbb\x0a\x33\x60\xad\x36\x21\x0e\x15\x3a\xc7\xb9\x35\xf5\x26\x9f\x69\x9f\x54\xf2\x90\x4a\x2d\x53\x6f\xb6\x1c\xad\x5d\x2e\x04\x34\x1d\xc6\x37\xb4\x4a\x74\xfc\xa1\x8d\xc6\xad\x4b\xe1\x3f\x85\xc1\x30\x35\x76\x3d\x04\xe9\x00\xcf\xd9\xb1\xc2\xe1\xa8\x8a\xaa\xab\xb1\x94\x96\x9c\xeb\x2f\x72\x6c\xb6\xa4\x07\x6b\x5c\x90\xd2\xee\x8e\x81\x71\x62\x09\x99\x26\x27\xc6\x65\x02\xf0\x18\x56\xf4\x3e\x96\x8b\x26\x00\xd9\x34\x98\x7a\xf5\x57\x7e\xba\xec\x4e\x3d\x0b\x0e\xa3\xb2\xd9\x20\x6b\x3b\xd8\x47\x01\x5c\x8e\xff\xf8\xd7\xfb\xb7\xb0\x07\x53\x33\x1a\xcd\x54\x86\x52\x6e\x5e\xbe\xbe\xa1\x0d\x64\x3b\xb2\xae\x57\x7c\x61\xb4\xe7\xa8\x37\x54\x98\xcd\xc0\x5d\xaa\x24\xc7\x58\x56\xbd\xb8\x7c\x4f\x92\x73\x78\xf8\xc9\x8c\x0c\x2a\x8f\x47\xe3\x4f\x16\x71\xdf\x15\x06\x36\xfe\xa2\x9a\x76\x0e\x06\x35\x65\xa3\x33\x7d\x83\x22\xac\xf8\xaf\x45\x4b\xfd\x77\x34\x0e\x87\xf4\x2f\x99\x05\x7e\xa8\xb6\x90\x86\xd9\xf0\x4b\xca\x7f\x60\xec\xd1\xfa\xe0\xe3\x7a\xc2\xce\x3f\x03\x00\x00\xff\xff\xc4\x5a\xa1\x33\x04\x0f\x00\x00")

func schemaGraphqlBytes() ([]byte, error) {
	return bindataRead(
		_schemaGraphql,
		"schema.graphql",
	)
}

func schemaGraphql() (*asset, error) {
	bytes, err := schemaGraphqlBytes()
	if err != nil {
		return nil, err
	}

	info := bindataFileInfo{name: "schema.graphql", size: 3844, mode: os.FileMode(420), modTime: time.Unix(1586895262, 0)}
	a := &asset{bytes: bytes, info: info}
	return a, nil
}

// Asset loads and returns the asset for the given name.
// It returns an error if the asset could not be found or
// could not be loaded.
func Asset(name string) ([]byte, error) {
	cannonicalName := strings.Replace(name, "\\", "/", -1)
	if f, ok := _bindata[cannonicalName]; ok {
		a, err := f()
		if err != nil {
			return nil, fmt.Errorf("Asset %s can't read by error: %v", name, err)
		}
		return a.bytes, nil
	}
	return nil, fmt.Errorf("Asset %s not found", name)
}

// MustAsset is like Asset but panics when Asset would return an error.
// It simplifies safe initialization of global variables.
func MustAsset(name string) []byte {
	a, err := Asset(name)
	if err != nil {
		panic("asset: Asset(" + name + "): " + err.Error())
	}

	return a
}

// AssetInfo loads and returns the asset info for the given name.
// It returns an error if the asset could not be found or
// could not be loaded.
func AssetInfo(name string) (os.FileInfo, error) {
	cannonicalName := strings.Replace(name, "\\", "/", -1)
	if f, ok := _bindata[cannonicalName]; ok {
		a, err := f()
		if err != nil {
			return nil, fmt.Errorf("AssetInfo %s can't read by error: %v", name, err)
		}
		return a.info, nil
	}
	return nil, fmt.Errorf("AssetInfo %s not found", name)
}

// AssetNames returns the names of the assets.
func AssetNames() []string {
	names := make([]string, 0, len(_bindata))
	for name := range _bindata {
		names = append(names, name)
	}
	return names
}

// _bindata is a table, holding each asset generator, mapped to its name.
var _bindata = map[string]func() (*asset, error){
	"schema.graphql": schemaGraphql,
}

// AssetDir returns the file names below a certain
// directory embedded in the file by go-bindata.
// For example if you run go-bindata on data/... and data contains the
// following hierarchy:
//     data/
//       foo.txt
//       img/
//         a.png
//         b.png
// then AssetDir("data") would return []string{"foo.txt", "img"}
// AssetDir("data/img") would return []string{"a.png", "b.png"}
// AssetDir("foo.txt") and AssetDir("notexist") would return an error
// AssetDir("") will return []string{"data"}.
func AssetDir(name string) ([]string, error) {
	node := _bintree
	if len(name) != 0 {
		cannonicalName := strings.Replace(name, "\\", "/", -1)
		pathList := strings.Split(cannonicalName, "/")
		for _, p := range pathList {
			node = node.Children[p]
			if node == nil {
				return nil, fmt.Errorf("Asset %s not found", name)
			}
		}
	}
	if node.Func != nil {
		return nil, fmt.Errorf("Asset %s not found", name)
	}
	rv := make([]string, 0, len(node.Children))
	for childName := range node.Children {
		rv = append(rv, childName)
	}
	return rv, nil
}

type bintree struct {
	Func     func() (*asset, error)
	Children map[string]*bintree
}

var _bintree = &bintree{nil, map[string]*bintree{
	"schema.graphql": &bintree{schemaGraphql, map[string]*bintree{}},
}}

// RestoreAsset restores an asset under the given directory
func RestoreAsset(dir, name string) error {
	data, err := Asset(name)
	if err != nil {
		return err
	}
	info, err := AssetInfo(name)
	if err != nil {
		return err
	}
	err = os.MkdirAll(_filePath(dir, filepath.Dir(name)), os.FileMode(0755))
	if err != nil {
		return err
	}
	err = ioutil.WriteFile(_filePath(dir, name), data, info.Mode())
	if err != nil {
		return err
	}
	err = os.Chtimes(_filePath(dir, name), info.ModTime(), info.ModTime())
	if err != nil {
		return err
	}
	return nil
}

// RestoreAssets restores an asset under the given directory recursively
func RestoreAssets(dir, name string) error {
	children, err := AssetDir(name)
	// File
	if err != nil {
		return RestoreAsset(dir, name)
	}
	// Dir
	for _, child := range children {
		err = RestoreAssets(dir, filepath.Join(name, child))
		if err != nil {
			return err
		}
	}
	return nil
}

func _filePath(dir, name string) string {
	cannonicalName := strings.Replace(name, "\\", "/", -1)
	return filepath.Join(append([]string{dir}, strings.Split(cannonicalName, "/")...)...)
}
