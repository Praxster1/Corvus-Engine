target_sources(Engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Pipeline.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Pipeline.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Device.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Device.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Instance.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Instance.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/DebugMessenger.h
        ${CMAKE_CURRENT_SOURCE_DIR}/DebugMessenger.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/QueueFamilyIndices.h
        ${CMAKE_CURRENT_SOURCE_DIR}/QueueFamilyIndices.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/SwapChain.h
        ${CMAKE_CURRENT_SOURCE_DIR}/SwapChain.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Vertex.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Vertex.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/VertexBuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/VertexBuffer.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/BufferUtils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/BufferUtils.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/IndexBuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/IndexBuffer.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/UniformBuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/UniformBuffer.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Shader.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp
)

