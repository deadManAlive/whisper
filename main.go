// Backend prototyping in go.
// Just because generating and uploading SPIFF and compiling and flasing ESP32 is a pain
package main

import "github.com/gin-gonic/gin"

func main() {
	router := gin.Default()

	router.Static("/", "./public")
	router.Run()
}
